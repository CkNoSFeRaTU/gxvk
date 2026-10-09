#pragma once

#include <memory>
#include <mutex>

#include "../dxvk/hud/dxvk_hud.h"

#include "../dxvk/dxvk_latency.h"
#include "../dxvk/dxvk_swapchain_blitter.h"

#include "../util/sync/sync_signal.h"

#include "../wsi/wsi_window.h"
#include "../wsi/wsi_monitor.h"

namespace dxvk {

  class DxvkAdapter;
  class DxvkDevice;
  class GlideDevice;
  class GlideCommonTexture;

  struct SwapChainDesc {
    FxU32 BackBufferHeight;
    FxU32 BackBufferWidth;
    FxU32 ScreenHeight;
    FxU32 ScreenWidth;
    FxU32 RefreshRate;
    FxU32 BufferCount;
    FxU32 AuxBufferCount;
    GrColorFormat_t ColorFormat;
  };

  class GlideSwapChain final {
    constexpr static uint32_t DefaultFrameLatency = 1;

  public:
    GlideSwapChain(HWND window, GlideDevice *parent, SwapChainDesc desc);
    ~GlideSwapChain();

    void BufferSwap(bool doSwap = true, FxU32 interval = 0);
    FxBool GetBuffer(GrBuffer_t Type, Rc<GlideCommonTexture>& ppBackBuffer);
    void Reset(HWND window, SwapChainDesc *desc);

    force_inline void incRef() {
      m_refCount.fetch_add(1u);
    }

    force_inline void decRef() {
      if (m_refCount.fetch_sub(1u) == 1u)
        delete this;
    }

    void DestroyBackBuffers();
    VkSurfaceFormatKHR GetBufferFormat(GrBuffer_t buffer);

    void UpdateTargetFrameRate(uint32_t SyncInterval);
    void UpdateWindowedRefreshRate();
    void SyncFrameLatency();

  private:
    void ChangeDisplayMode();

    void CreateFrameLatencyEvent();
    void CreatePresenter();
    void CreateBackBuffers();
    void CreateBlitter();
    void DestroyFrameLatencyEvent();
    void DestroyLatencyTracker();
    void UpdateRects(SwapChainDesc *desc);

    uint32_t GetActualFrameLatency();

    SwapChainDesc m_desc;
    GlideDevice* m_parent;
    Rc<DxvkDevice> m_device;
    HMONITOR                  m_monitor  = nullptr;
    wsi::DxvkWindowState      m_windowState;

    HWND m_window;

    std::atomic<uint32_t>     m_refCount = { 0u };
    VkColorSpaceKHR           m_colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    small_vector<Rc<GlideCommonTexture>, GLIDE_MAX_SWAP_CHAIN_BUFFERS> m_backBuffers;
    small_vector<Rc<GlideCommonTexture>, GLIDE_MAX_AUX_BUFFERS> m_auxBuffers;

    Rc<DxvkSwapchainBlitter>  m_blitter;
    Rc<DxvkLatencyTracker>    m_latencyTracker;
    Rc<hud::HudLatencyItem>   m_latencyHud;
    Rc<Presenter>             m_presenter;
    RECT                      m_srcRect;
    RECT                      m_dstRect;

    double                    m_targetFrameRate = 0.0;
    double                    m_displayRefreshRate = 0.0;
    bool                      m_displayRefreshRateDirty = true;

    uint64_t                  m_frameId = GLIDE_MAX_SWAP_CHAIN_BUFFERS;
    uint32_t                  m_frameLatency = DefaultFrameLatency;
    uint32_t                  m_frameLatencyCap = 0;
    Rc<sync::CallbackFence>   m_frameLatencySignal;

  };

}
