#include "glide_device.h"
#include "glide_swapchain.h"
#include "glide_texture.h"

#include "../dxvk/dxvk_latency_builtin.h"
#include "../dxvk/hud/dxvk_hud.h"

#include "../util/util_win32_compat.h"

#include <algorithm>

namespace dxvk {

  static Rc<DxvkSwapchainBlitter> i_blitter = nullptr;
  static Rc<hud::Hud> i_hud = nullptr;
  static WNDPROC wndProc = nullptr;
  LRESULT CALLBACK HookedWindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (unlikely(i_blitter != nullptr && (GetKeyState(VK_SHIFT) & 0x8000))) {
      if (unlikely(i_hud && msg == WM_SYSKEYDOWN && wParam == VK_F10))
        i_hud->GetOptions()->opacity = std::max(i_hud->GetOptions()->opacity - 0.1f, 0.0f);
      else if (unlikely(i_hud && msg == WM_KEYDOWN && wParam == VK_F11))
        i_hud->GetOptions()->opacity = std::min(i_hud->GetOptions()->opacity + 0.1f, 1.0f);
      else if (unlikely(i_blitter && msg == WM_KEYDOWN && wParam == VK_F12)) {
        if (unlikely(i_hud && i_hud->GetOptions()->opacity == 0.0f))
          i_hud->GetOptions()->opacity = 0.1f;

        i_blitter->hudStateToggle();
      }

      return 0;
    }

    return CallWindowProcW(wndProc, window, msg, wParam, lParam);
  }

  GlideSwapChain::GlideSwapChain(HWND window, GlideDevice *parent, SwapChainDesc desc)
  : m_parent(parent)
  , m_device(m_parent->GetDXVKDevice())
  , m_window(window) {

    UpdateRects(&desc);

    CreateFrameLatencyEvent();
    CreatePresenter();
    CreateBackBuffers();
    CreateBlitter();

    m_monitor = wsi::getWindowMonitor(m_window);
    if (unlikely(!m_monitor))
      return;

    ChangeDisplayMode();

    i_blitter = m_blitter;
    if (likely(m_window && wndProc == nullptr))
      wndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HookedWindowProc)));
  };

  GlideSwapChain::~GlideSwapChain() {
    // Avoids hanging when in this state, see comment
    // in DxvkDevice::~DxvkDevice.
    if (this_thread::isInModuleDetachment())
      return;

    if (likely(m_window && wndProc != nullptr)) {
      SetWindowLongPtrW(m_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProc));
      wndProc = nullptr;
    }
    i_blitter = nullptr;
    i_hud = nullptr;

    m_presenter->destroyResources();

    DestroyLatencyTracker();
    DestroyFrameLatencyEvent();
    DestroyBackBuffers();

    if (!m_monitor)
      return;

    wsi::restoreDisplayMode();
  }

  void GlideSwapChain::UpdateTargetFrameRate(uint32_t SyncInterval) {
    double frameRate = double(m_parent->GetOptions()->maxFrameRate);

    if (frameRate != -1.0) {
      if (frameRate == 0.0 && SyncInterval) {
        bool engageLimiter = SyncInterval > 1u || m_monitor ||
          m_device->config().latencySleep == Tristate::True;

        if (engageLimiter)
          frameRate = -m_displayRefreshRate / double(SyncInterval);
      }

      m_presenter->setFrameRateLimit(frameRate, GetActualFrameLatency());
      m_targetFrameRate = frameRate;
    }
  }

  void GlideSwapChain::UpdateWindowedRefreshRate() {
    // Ignore call if we are in fullscreen mode and
    // know the active display mode already anyway
    if (!m_displayRefreshRateDirty || m_monitor)
      return;

    m_displayRefreshRate = 0.0;
    m_displayRefreshRateDirty = false;

    HMONITOR monitor = wsi::getWindowMonitor(m_window);

    if (!monitor)
      return;

    wsi::WsiMode mode = { };

    if (!wsi::getCurrentDisplayMode(monitor, &mode))
      return;

    if (mode.refreshRate.denominator) {
      m_displayRefreshRate = double(mode.refreshRate.numerator)
                           / double(mode.refreshRate.denominator);
    }
  }

  void GlideSwapChain::SyncFrameLatency() {
    // Wait for the sync event so that we respect the maximum frame latency
    m_frameLatencySignal->wait(m_frameId - GetActualFrameLatency());
  }

  uint32_t GlideSwapChain::GetActualFrameLatency() {
    uint32_t maxFrameLatency = GLIDE_MAX_SWAP_CHAIN_BUFFERS;

    if (m_frameLatencyCap)
      maxFrameLatency = std::min(maxFrameLatency, m_frameLatencyCap);

    maxFrameLatency = std::min(maxFrameLatency, m_desc.BufferCount);
    return maxFrameLatency;
  }

  void GlideSwapChain::ChangeDisplayMode() {
    wsi::WsiMode mode;

    mode.width = m_desc.ScreenWidth;
    mode.height = m_desc.ScreenHeight;
    mode.bitsPerPixel = 32;
    mode.refreshRate = wsi::WsiRational{m_desc.RefreshRate, 1};
    mode.interlaced = false;

    wsi::setWindowMode(m_monitor, m_window, &m_windowState, mode);
    wsi::enterFullscreenMode(m_monitor, m_window, &m_windowState, true);

    m_displayRefreshRate = 0.0;

    if (wsi::getCurrentDisplayMode(m_monitor, &mode)) {
      m_displayRefreshRate = double(mode.refreshRate.numerator)
                           / double(mode.refreshRate.denominator);
    }

    m_displayRefreshRateDirty = false;
  }

  void GlideSwapChain::CreateFrameLatencyEvent() {
    m_frameLatencySignal = new sync::CallbackFence(m_frameId);
  }

  VkSurfaceFormatKHR GlideSwapChain::GetBufferFormat(GrBuffer_t buffer) {
    switch (buffer) {
      default:
        Logger::warn(str::format("GlideSwapChain: Unexpected format: ", m_desc.ColorFormat));
        [[fallthrough]];

      case GR_BUFFER_ALPHABUFFER:
        return { VK_FORMAT_R16_UINT, m_colorSpace };
      case GR_BUFFER_BACKBUFFER:
      case GR_BUFFER_FRONTBUFFER:
        return { VK_FORMAT_R8G8B8A8_UNORM, m_colorSpace };
      case GR_BUFFER_AUXBUFFER:
      case GR_BUFFER_DEPTHBUFFER:
        return { VK_FORMAT_D16_UNORM, m_colorSpace };
    }
  }

  FxBool GlideSwapChain::GetBuffer(GrBuffer_t Type, Rc<GlideCommonTexture>& ppBackBuffer) {
    if (Type == GR_BUFFER_FRONTBUFFER && m_backBuffers.size() > 0) {
      ppBackBuffer = m_backBuffers[0];
      return FXTRUE;
    }
    else if (Type == GR_BUFFER_BACKBUFFER && m_backBuffers.size() > 1) {
      ppBackBuffer = m_backBuffers[1];
      return FXTRUE;
    }
    else if ((Type == GR_BUFFER_AUXBUFFER) && m_auxBuffers.size() > 0) {
      ppBackBuffer = m_auxBuffers[0];
      return FXTRUE;
    }

    return FXFALSE;
  }

  void GlideSwapChain::CreatePresenter() {
    PresenterDesc presenterDesc = { };
    presenterDesc.deferSurfaceCreation = false;

    m_presenter = new Presenter(m_device, m_frameLatencySignal, presenterDesc, [
      cAdapter = m_device->adapter(),
      cWindow = m_window
    ] (VkSurfaceKHR* surface) {
      auto vki = cAdapter->vki();

      return wsi::createSurface(cWindow,
        vki->getLoaderProc(),
        vki->instance(),
        surface);
    });

    m_presenter->setSurfaceFormat(GetBufferFormat(GR_BUFFER_BACKBUFFER));
    m_presenter->setSurfaceExtent({m_desc.ScreenWidth, m_desc.ScreenHeight});
    m_presenter->setFrameRateLimit(m_targetFrameRate, GetActualFrameLatency());

    m_latencyTracker = m_device->createLatencyTracker(m_presenter);
  }

  void GlideSwapChain::CreateBackBuffers() {
    // Explicitly destroy current swap image before
    // creating a new one to free up resources
    DestroyBackBuffers();

    const uint32_t auxCount = m_desc.AuxBufferCount;
    const uint32_t bufferCount = m_desc.BufferCount;

    m_backBuffers.reserve(bufferCount);
    m_auxBuffers.reserve(auxCount);

    // Create new back buffer
    GLIDE_COMMON_TEXTURE_DESC desc;
    desc.Width              = std::max(m_desc.BackBufferWidth,  1u);
    desc.Height             = std::max(m_desc.BackBufferHeight, 1u);
    desc.MipMaps            = 1;
    desc.Format             = GetBufferFormat(GR_BUFFER_BACKBUFFER).format;
    desc.Type               = GR_BUFFER_BACKBUFFER;

    for (uint32_t i = 0; i < bufferCount; i++) {
      Rc<GlideCommonTexture> texture;
      try {
        desc.Id = i;
        texture = new GlideCommonTexture(m_parent, &desc);
        // m_parent->IncrementLosableCounter();
      } catch (const DxvkError& e) {
        DestroyBackBuffers();
        Logger::err(e.message());
        exit(0);
      }

      m_backBuffers.emplace_back(std::move(texture));
    }

    // Initialize the image so that we can use it. Clearing
    // to black prevents garbled output for the first frame.
    small_vector<Rc<DxvkImage>, GLIDE_MAX_SWAP_CHAIN_BUFFERS> bufImages;

    for (size_t i = 0; i < m_backBuffers.size(); i++)
      bufImages.push_back(m_backBuffers[i]->GetImage());

    // Create new aux buffer
    desc.Width              = std::max(m_desc.BackBufferWidth,  1u);
    desc.Height             = std::max(m_desc.BackBufferHeight, 1u);
    desc.MipMaps            = 1;
    desc.Format             = GetBufferFormat(GR_BUFFER_DEPTHBUFFER).format;
    desc.Type               = GR_BUFFER_DEPTHBUFFER;

    for (uint32_t i = 0; i < auxCount; i++) {
      Rc<GlideCommonTexture> texture;
      try {
        desc.Id = i;
        texture = new GlideCommonTexture(m_parent, &desc);
      } catch (const DxvkError& e) {
        DestroyBackBuffers();
        Logger::err(e.message());
        exit(0);
      }

      m_auxBuffers.emplace_back(std::move(texture));
    }

    // Initialize the image so that we can use it. Clearing
    // to black prevents garbled output for the first frame.
    small_vector<Rc<DxvkImage>, GLIDE_MAX_AUX_BUFFERS> auxImages;

    for (size_t i = 0; i < m_auxBuffers.size(); i++)
      auxImages.push_back(m_auxBuffers[i]->GetImage());

    m_parent->InjectCs([
      bufImages = std::move(bufImages),
      auxImages = std::move(auxImages)
    ] (DxvkContext* ctx) {
      for (size_t i = 0; i < bufImages.size(); i++) {
        ctx->initImage(bufImages[i], VK_IMAGE_LAYOUT_UNDEFINED);
      }
      for (size_t i = 0; i < auxImages.size(); i++) {
        ctx->initImage(auxImages[i], VK_IMAGE_LAYOUT_UNDEFINED);
      }
      Logger::info(str::format("Buffers initialized: ", bufImages.size(), " back, ", auxImages.size(), " aux"));
    });
  }

  void GlideSwapChain::CreateBlitter() {
    Rc<hud::Hud> hud = hud::Hud::createHud(m_device);

    if (hud) {
      if (hud->GetOptions()->scale == 1.0f)
        hud->GetOptions()->scale = float(m_desc.ScreenHeight) / 768.0f;
      hud->addItem<hud::HudClientApiItem>("api", 1, m_parent->GetAPI() + " (" + m_parent->GetBoardConfiguration()->name + ")");

      if (m_latencyTracker)
        m_latencyHud = hud->addItem<hud::HudLatencyItem>("latency", 4);
    }

    i_hud = std::move(hud);
    m_blitter = new DxvkSwapchainBlitter(m_device, i_hud);
  }

  void GlideSwapChain::DestroyBackBuffers() {
    m_auxBuffers.clear();
    m_backBuffers.clear();
  }

  void GlideSwapChain::BufferSwap(bool doSwap, FxU32 interval) {
    const int presentInterval = m_parent->GetOptions()->presentInterval;
    if (presentInterval >= 0)
      interval = presentInterval;

    m_presenter->setSyncInterval(interval);
    UpdateWindowedRefreshRate();
    UpdateTargetFrameRate(interval);

    m_parent->EndFrame(m_latencyTracker);
    m_parent->Flush();

    if (m_latencyTracker)
      m_latencyTracker->notifyCpuPresentBegin(m_frameId + 1u);

    // Retrieve the image and image view to present
    VkResult status = VK_SUCCESS;

    //Rc<DxvkImage> swapImage = m_backBuffers[0]->GetImage();
    Rc<DxvkImageView> swapImageView = m_backBuffers[0]->GetView(0);

    // Presentation semaphores and WSI swap chain image
    PresenterSync sync = { };
    Rc<DxvkImage> backBuffer;

    status = m_presenter->acquireNextImage(sync, backBuffer);

    if (status >= 0 && status != VK_NOT_READY) {
      VkRect2D srcRect = {
        {  int32_t(m_srcRect.left),                    int32_t(m_srcRect.top)                    },
        { uint32_t(m_srcRect.right - m_srcRect.left), uint32_t(m_srcRect.bottom - m_srcRect.top) } };

      VkRect2D dstRect = {
        {  int32_t(m_dstRect.left),                    int32_t(m_dstRect.top)                    },
        { uint32_t(m_dstRect.right - m_dstRect.left), uint32_t(m_dstRect.bottom - m_dstRect.top) } };

      // Bump frame ID
      m_frameId += 1;

      // Present from CS thread so that we don't
      // have to synchronize with it first.
      DxvkImageViewKey viewInfo;
      viewInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.usage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      viewInfo.format     = backBuffer->info().format;
      viewInfo.aspects    = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.mipIndex   = 0u;
      viewInfo.mipCount   = 1u;
      viewInfo.layerIndex = 0u;
      viewInfo.layerCount = 1u;

      m_parent->EmitCs([
        cDevice         = m_device,
        cPresenter      = m_presenter,
        cBlitter        = m_blitter,
        cColorSpace     = m_colorSpace,
        cSrcView        = std::move(swapImageView),
        cSrcRect        = srcRect,
        cDstView        = std::move(backBuffer->createView(viewInfo)),
        cDstRect        = dstRect,
        cSync           = sync,
        cFrameId        = m_frameId,
        cLatency        = m_latencyTracker
      ] (DxvkContext* ctx) {
        // Update back buffer color space as necessary
        if (cSrcView->image()->info().colorSpace != cColorSpace) {
          DxvkImageUsageInfo usage = { };
          usage.colorSpace = cColorSpace;

          ctx->ensureImageCompatibility(cSrcView->image(), usage);
        }

        // Blit back buffer onto Vulkan swap chain
        auto contextObjects = ctx->beginExternalRendering();
        cBlitter->present(contextObjects, VkClearColorValue(), cDstView, cDstRect, cSrcView, cSrcRect);

        // Submit command list and present
        ctx->synchronizeWsi(cSync);
        ctx->flushCommandList(nullptr, nullptr);

        cDevice->presentImage(cPresenter, cLatency, cFrameId, 0, nullptr, nullptr);
      });

      m_parent->FlushCsChunk();
    }

    if (m_latencyTracker) {
      if (status == VK_SUCCESS)
        m_latencyTracker->notifyCpuPresentEnd(m_frameId);
      else
        m_latencyTracker->discardTimings();
    }

    SyncFrameLatency();

    DxvkLatencyStats latencyStats = { };

    if (m_latencyTracker && status == VK_SUCCESS) {
      latencyStats = m_latencyTracker->getStatistics(m_frameId);
      m_latencyTracker->sleepAndBeginFrame(m_frameId + 1, std::abs(m_targetFrameRate));

      m_parent->BeginFrame(m_latencyTracker, m_frameId + 1u);
    }

    if (m_latencyHud)
      m_latencyHud->accumulateStats(latencyStats);

    if (doSwap) {
      // Rotate swap chain buffers so that the back
      // buffer at index 0 becomes the front buffer.
      uint32_t rotatingBufferCount = m_backBuffers.size();

      // Backbuffer 0 is the one that gets copied to the Vulkan swapchain backbuffer.
      // => m_backBuffers[1] is the next one that gets presented
      // and the currente m_backBuffers[0] ends up at the end of the vector.
      for (uint32_t i = 1; i < rotatingBufferCount; i++)
        m_backBuffers[i]->Swap(m_backBuffers[i - 1].ptr());
    }
  }

  void GlideSwapChain::DestroyFrameLatencyEvent() {
  }

  void GlideSwapChain::DestroyLatencyTracker() {
    if (!m_latencyTracker)
      return;

    m_parent->InjectCs([
      cTracker = std::move(m_latencyTracker)
    ] (DxvkContext* ctx) {
      ctx->endLatencyTracking(cTracker);
    });
  }

  void GlideSwapChain::Reset(HWND window, SwapChainDesc *desc) {
    if (unlikely(desc == nullptr))
      return;

    if (m_window != window) {
      m_window = window;
      m_monitor = wsi::getWindowMonitor(window);

      if (unlikely(!m_monitor))
          return;
    }

#if FULL_RESET
    m_presenter->invalidateSurface();
    m_presenter->destroyResources();

    DestroyFrameLatencyEvent();
    DestroyLatencyTracker();
    DestroyBackBuffers();
#endif

    UpdateRects(desc);

#if FULL_RESET
    CreateFrameLatencyEvent();
    CreatePresenter();
#endif

    CreateBackBuffers();

#if FULL_RESET
    CreateBlitter();
#endif

    ChangeDisplayMode();
  }

  void GlideSwapChain::UpdateRects(SwapChainDesc *desc) {
    m_desc = *desc;

    m_srcRect.top    = 0;
    m_srcRect.left   = 0;
    m_srcRect.right  = desc->BackBufferWidth;
    m_srcRect.bottom = desc->BackBufferHeight;

    const float scaleX = static_cast<float>(desc->ScreenWidth) / static_cast<float>(desc->BackBufferWidth);
    const float scaleY = static_cast<float>(desc->ScreenHeight) / static_cast<float>(desc->BackBufferHeight);
    const float scale = std::min(scaleX, scaleY);
    const float newWidth = static_cast<float>(desc->BackBufferWidth) * scale;
    const float newHeight = static_cast<float>(desc->BackBufferHeight) * scale;

    m_dstRect.left   = static_cast<float>(desc->ScreenWidth - newWidth) * 0.5f;
    m_dstRect.top    = static_cast<float>(desc->ScreenHeight - newHeight) * 0.5f;
    m_dstRect.right  = m_dstRect.left + newWidth;
    m_dstRect.bottom = m_dstRect.top + newHeight;
  }

}
