#include "../dxvk/dxvk_adapter.h"
#include "../dxvk/dxvk_device.h"
#include "../util/util_singleton.h"
#include "../wsi/wsi_window.h"

#include "glide_device.h"
#include "glide_texture.h"
#include "glide_utils.h"

#include <algorithm>
#include <cstring>

namespace dxvk {

  static float glideFogIndexToW[FOG_TABLE_ENTRIES_COUNT];
  Singleton<DxvkInstance> g_dxvkInstance;

  GlideDevice::GlideDevice(GLIDEAPI api)
  : m_api(api)
  , m_instance(g_dxvkInstance.acquire(0))
  , m_adapter(m_instance->enumAdapters(0))
  , m_device(m_adapter->createDevice())
  , m_shaders(this)
  , m_glideOptions(m_instance->config())
  , m_boardConfiguration(UtilGetBoardConfiguration(api, &m_glideOptions))
  , m_hwConfiguration(UtilGetHWConfiguration(&m_boardConfiguration))
  , m_multithread(m_glideOptions.multithread)
  , m_csThread{m_device, m_device->createContext()}
  , m_csChunk(AllocCsChunk())
  , m_stagingBuffer(m_device, StagingBufferSize)
  , m_stagingBufferFence(new sync::Fence())
  , m_submissionFence(new sync::Fence())
  , m_flushTracker(GetMaxFlushType()) {
    GlideDeviceLock lock = LockDevice();

    Logger::info(str::format("Board: ", m_boardConfiguration.name));
    for (int idx = 0; idx < m_boardConfiguration.tmuCount; idx++) {
      m_tmu[idx] = new GlideTMU(this, static_cast<GrChipID_t>(idx));
      Logger::info(str::format("TMU ", idx, " started with ", m_boardConfiguration.tmuRam, "MB of RAM"));
    }

    Reset();

    EmitCs([
      cDevice = m_device
    ] (DxvkContext *ctx) {
      ctx->beginRecording(cDevice->createCommandList());

      // Disable logic op once and for all.
      DxvkLogicOpState loState = { };
      ctx->setLogicOpState(loState);
    });

    SynchronizeCsThread(DxvkCsThread::SynchronizeAll);

    // Check for VK_EXT_depth_bias_control and set up initial state
    m_depthBiasRepresentation = { VK_DEPTH_BIAS_REPRESENTATION_LEAST_REPRESENTABLE_VALUE_FORMAT_EXT, false };
    if (m_device->features().extDepthBiasControl.depthBiasControl) {
      if (m_device->features().extDepthBiasControl.depthBiasExact)
        m_depthBiasRepresentation.depthBiasExact = true;

      if (m_device->features().extDepthBiasControl.floatRepresentation) {
        m_depthBiasRepresentation.depthBiasRepresentation = VK_DEPTH_BIAS_REPRESENTATION_FLOAT_EXT;
        m_depthBiasScale = 1.0f;
      }
      else if (m_device->features().extDepthBiasControl.leastRepresentableValueForceUnormRepresentation)
        m_depthBiasRepresentation.depthBiasRepresentation = VK_DEPTH_BIAS_REPRESENTATION_LEAST_REPRESENTABLE_VALUE_FORCE_UNORM_EXT;
    }

    EmitCs([
      cFS             = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS             = m_shaders.GetShader<GlideShaderType::VertexShader>(),
      cRepresentation = m_depthBiasRepresentation
    ] (DxvkContext* ctx) mutable {
      ctx->bindShader<VK_SHADER_STAGE_FRAGMENT_BIT>(std::move(cFS));
      ctx->bindShader<VK_SHADER_STAGE_VERTEX_BIT>(std::move(cVS));
      ctx->setDepthBiasRepresentation(cRepresentation);
    });

    for (int idx = 0; idx < FOG_TABLE_ENTRIES_COUNT; idx++) {
      glideFogIndexToW[idx] = std::powf(2.0f, 3.0f + static_cast<float>(idx >> 2)) / (8.0f - static_cast<float>(idx & 3));
    }
  }

  GlideDevice::~GlideDevice() {
    GlideDeviceLock lock = LockDevice();

    // Avoids hanging when in this state, see comment
    // in DxvkDevice::~DxvkDevice.
    if (this_thread::isInModuleDetachment())
      return;

    if (m_swapchain != nullptr)
      delete m_swapchain;

    ExecuteFlush();
    SynchronizeCsThread(DxvkCsThread::SynchronizeAll);

    m_device->waitForIdle();
  }

  FxBool GlideDevice::WindowOpen(HWND hWnd,
                    GrScreenResolution_t resolution,
                    GrScreenRefresh_t refreshRate,
                    GrColorFormat_t colorFormat,
                    GrOriginLocation_t originLocation,
                    int nColBuffers, int nAuxBuffers) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(nColBuffers < GLIDE_MIN_SWAP_CHAIN_BUFFERS || nColBuffers > GLIDE_MAX_SWAP_CHAIN_BUFFERS)) {
      Logger::err("Expected double or tripple-buffering");
      return FXFALSE;
    }

    if (unlikely(nAuxBuffers > GLIDE_MAX_AUX_BUFFERS)) {
      Logger::err("Expected no or single aux buffer");
      return FXFALSE;
    }

    if (!UtilResolutionToDimensions(resolution, m_screen.width, m_screen.height, resolution > GR_RESOLUTION_NONE)) {
      Logger::err("Invalid resolution passed");
      return FXFALSE;
    }

    if (!UtilRefreshRateToNumber(refreshRate, m_screen.refreshRate, refreshRate > GR_REFRESH_NONE)) {
      Logger::err("Invalid refresh rate passed");
      return FXFALSE;
    }

    Logger::debug(str::format("Requested resolution: ", m_screen.width, "x", m_screen.height, "@", m_screen.refreshRate));

    SetColorFormat(colorFormat);
    SetOriginLocation(originLocation);
    SetViewport(0, 0, m_screen.width, m_screen.height);
    SetClipWindow(0, 0, m_screen.width, m_screen.height);

    m_colorBuffers = nColBuffers;
    m_auxBuffers = nAuxBuffers;
    m_window = hWnd;

    m_state.colorFormat = colorFormat;

    HMONITOR monitor = wsi::getWindowMonitor(hWnd);
    if (monitor == nullptr) {
      Logger::err("Window resides on unknown monitor");
      return FXFALSE;
    }

    SwapChainDesc desc;
    desc.BackBufferWidth = m_screen.width;
    desc.BackBufferHeight = m_screen.height;
    desc.RefreshRate = m_screen.refreshRate;
    desc.BufferCount = m_colorBuffers;
    desc.AuxBufferCount = m_auxBuffers;
    desc.ColorFormat = m_state.colorFormat;

    wsi::WsiMode mode;
    if (m_glideOptions.useMonitorResolution && wsi::getCurrentDisplayMode(monitor, &mode)) {
      desc.ScreenWidth = mode.width;
      desc.ScreenHeight = mode.height;
    } else {
      desc.ScreenWidth = desc.BackBufferWidth;
      desc.ScreenHeight = desc.BackBufferHeight;
    }

    if (m_swapchain == nullptr)
      m_swapchain = new GlideSwapChain(hWnd, this, std::move(desc));
    else {
      m_swapchain->Reset(hWnd, &desc);

      ExecuteFlush();
      SynchronizeCsThread(DxvkCsThread::SynchronizeAll);

      EmitCs([] (DxvkContext* ctx) {
        ctx->bindIndexBuffer(DxvkBufferSlice(), VK_INDEX_TYPE_UINT32);

        for (uint32_t i = 0; i < GLIDE_MAX_TMU; i++)
          ctx->bindVertexBuffer(i, DxvkBufferSlice(), 0);
      });
    }

    m_dirty.set(
      GlideDeviceDirtyFlag::Blend,
      GlideDeviceDirtyFlag::Depth,
      GlideDeviceDirtyFlag::FFShader,
      GlideDeviceDirtyFlag::Framebuffer,
      GlideDeviceDirtyFlag::PushDataVs,
      GlideDeviceDirtyFlag::PushDataFfvs,
      GlideDeviceDirtyFlag::PushDataFfps,
      GlideDeviceDirtyFlag::PushDataShared,
      GlideDeviceDirtyFlag::RasterizerState,
      GlideDeviceDirtyFlag::SpecializationEntries,
      GlideDeviceDirtyFlag::VertexLayout,
      GlideDeviceDirtyFlag::Viewport
    );

    UpdateFixedFunction();

    ExecuteFlush();
    SynchronizeCsThread(DxvkCsThread::SynchronizeAll);

    m_swapchain->BufferSwap();

    return FXTRUE;
  }

  void GlideDevice::WindowClose() {
    GlideDeviceLock lock = LockDevice();

    if (!m_glideOptions.deviceReset && m_swapchain != nullptr) {
      delete m_swapchain;
      m_swapchain = nullptr;
    }
  }

  FxBool GlideDevice::LinearBufferLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(info == nullptr))
      return FXFALSE;

    // skip checking if info->size == sizeof(GrLfbInfo_t) and write our size instead, Driver for some reason passes 20 on first call, but 28 on subsequent ones.
    info->size = sizeof(GrLfbInfo_t);

    // according to 2.4 docs only GR_BUFFER_FRONTBUFFER, GR_BUFFER_BACKBUFFER, and GR_BUFFER_AUXBUFFER are supported.
    if (unlikely(buffer < GR_BUFFER_FRONTBUFFER || buffer > GR_BUFFER_AUXBUFFER)) {
      static bool sWarnShown = false;
      if (!std::exchange(sWarnShown, true))
        Logger::warn(str::format("Unexpected buffer lock: ", buffer));
      return FXFALSE;
    }

    // according to 2.4 docs only GR_LFBWRITEMODE_565 for buffers and GR_LFBWRITEMODE_ZA16 for aux was supported for Voodoo Graphics/Rush.
    // but Diablo II wants rendering intros with GR_LFBWRITEMODE_8888 regardless of presented board!?;
/*
    const GrSstType boardType = m_boardConfiguration.type;
    if ((boardType == GR_SSTTYPE_Voodoo || boardType == GR_SSTTYPE_SST96) && writeMode != GR_LFBWRITEMODE_ANY) {
      if ((buffer == GR_BUFFER_FRONTBUFFER || buffer == GR_BUFFER_BACKBUFFER) && writeMode != GR_LFBWRITEMODE_565)
        return FXFALSE;

      if (buffer == GR_BUFFER_AUXBUFFER && writeMode != GR_LFBWRITEMODE_ZA16)
        return FXFALSE;
    }
*/

    if (origin == GR_ORIGIN_ANY)
      info->origin = m_state.originLocation;
    if (writeMode == GR_LFBWRITEMODE_ANY)
      info->writeMode = GR_LFBWRITEMODE_8888;
    else
      info->writeMode = writeMode;

    if (type != GR_LFB_WRITE_ONLY) {
      // TODO: readback
    }

    // TODO: non-UMA cards support framebuffers with only 1024 pixel width, 2048 bytes stride regardless of buffers resolution.
    // There are 2.11 games which rely on that.
    FxU32 blockSize = 0;
    UtilTextureFormatSize(UtilLFBWriteModeToIFormat(info->writeMode), &blockSize, nullptr, nullptr);

    auto& lfb = m_lfb[buffer];
    lfb.format = writeMode;
    lfb.width = m_state.viewport.extent.width;
    lfb.height = m_state.viewport.extent.height;
    lfb.pixelSize = blockSize >> 3;
    lfb.buffer.resize(lfb.width * lfb.height * lfb.pixelSize);

    info->strideInBytes = lfb.width * lfb.pixelSize;
    info->lfbPtr = lfb.buffer.data();

    return FXTRUE;
  }

  FxBool GlideDevice::LinearBufferUnlock(GrLock_t type, GrBuffer_t buffer) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(m_glideOptions.hackNoLFB)) {
      return FXTRUE;
    }

    // according to 2.4 docs only GR_BUFFER_FRONTBUFFER, GR_BUFFER_BACKBUFFER, and GR_BUFFER_AUXBUFFER are supported.
    if (unlikely(buffer < GR_BUFFER_FRONTBUFFER || buffer > GR_BUFFER_AUXBUFFER)) {
      static bool sWarnShown = false;
      if (!std::exchange(sWarnShown, true))
        Logger::warn(str::format("Unexpected buffer lock: ", buffer));
      return FXFALSE;
    }

    if (m_swapchain == nullptr)
      return FXFALSE;

    Rc<GlideCommonTexture> texture = nullptr;
    if (!m_swapchain->GetBuffer(buffer, texture))
      return FXFALSE;

    auto image = texture->GetImage();
    if (unlikely(image == nullptr))
      return FXFALSE;

    VkOffset3D DestOffset{};
    auto subresources = image->getAvailableSubresources();
    VkExtent3D dstTexLevelExtent = image->mipLevelExtent(subresources.baseMipLevel);
    auto formatInfo = image->formatInfo();
    VkOffset3D alignedDestOffset = {
      int32_t(alignDown(DestOffset.x, formatInfo->blockSize.width)),
      int32_t(alignDown(DestOffset.y, formatInfo->blockSize.height)),
      int32_t(alignDown(DestOffset.z, formatInfo->blockSize.depth))
    };
    VkExtent3D extentBlockCount = util::computeBlockCount(dstTexLevelExtent, formatInfo->blockSize);
    VkExtent3D alignedExtent = util::snapExtent3D(alignedDestOffset, extentBlockCount, dstTexLevelExtent);
    VkImageSubresourceLayers dstLayers = { subresources.aspectMask, subresources.baseMipLevel, subresources.baseArrayLayer, subresources.layerCount };

    auto& lfb = m_lfb[buffer];

    FxU32 components = 4, dstPitch = 0;
    size_t bufferSize = lfb.width * lfb.height * components;

    std::vector<uint8_t> pixelData;
    pixelData.resize(bufferSize);

    UtilConvertImageToRGBA(UtilLFBWriteModeToIFormat(lfb.format), lfb.buffer.data(), pixelData, components, dstPitch, lfb.width, lfb.height, nullptr);
    VkDeviceSize pitch = align(dstPitch, components);

    auto slice = m_stagingBuffer.alloc(bufferSize);
    util::packImageData(
      slice.mapPtr(0), pixelData.data(), extentBlockCount, components,
      pitch, pitch * lfb.height);

    EmitCs([
      cSrcSlice       = slice,
      cDstImage       = image,
      cDstLayers      = dstLayers,
      cDstLevelExtent = alignedExtent,
      cOffset         = alignedDestOffset,
      cColorFormat    = m_swapchain->GetBufferFormat(buffer).format
    ] (DxvkContext* ctx) mutable {
      ctx->copyBufferToImage(
        cDstImage,  cDstLayers,
        cOffset, cDstLevelExtent,
        cSrcSlice.buffer(), cSrcSlice.offset(),
        0, 0, cColorFormat);
    });

    if (buffer == GR_BUFFER_FRONTBUFFER)
      m_swapchain->BufferSwap(false);

    ConsiderFlush(GpuFlushType::ImplicitWeakHint);

    return FXTRUE;
  }

  void GlideDevice::BufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth) {
    GlideDeviceLock lock = LockDevice();
    if (unlikely(m_glideOptions.hackNoClears)) {
      return;
    }

    if (m_state.colorMask.color) {
      Rc<GlideCommonTexture> textureRT = nullptr;
      if (m_swapchain->GetBuffer(m_state.renderBuffer, textureRT)) {
        VkClearColorValue clearValue;
        clearValue.float32[0] = float((color >> 0) & 0xFF) / 255.0;
        clearValue.float32[1] = float((color >> 8) & 0xFF) / 255.0;
        clearValue.float32[2] = float((color >> 16) & 0xFF) / 255.0;
        clearValue.float32[3] = float(alpha) / 255.0;
        VkClearValue clearBlock = { };
        clearBlock.color = util::encodeClearBlockValue(textureRT->GetImage()->info().format, clearValue);

        Rc<DxvkImageView> view = textureRT->GetView(0);
        VkImageSubresourceRange subresources = textureRT->GetImage()->getAvailableSubresources();

        EmitCs([
          cView = view,
          cSubResource = subresources,
          cClearValue = clearBlock
        ](DxvkContext* ctx) {
          DxvkAttachment attachment = {};
          attachment.view = cView;
          VkImageAspectFlags cAspectMask = cSubResource.aspectMask;
          ctx->clearRenderTarget(attachment, cAspectMask, cClearValue, 0u);
        });
      }

      Logger::debug(str::format("buffer ", m_state.renderBuffer, " cleared"));
    }

    if (likely(!m_glideOptions.hackNoDepth)) {
      if (m_state.depthMode != GR_DEPTHBUFFER_DISABLE) {
        Rc<GlideCommonTexture> textureDepth = nullptr;
        if (m_swapchain->GetBuffer(GR_BUFFER_AUXBUFFER, textureDepth)) {
          VkClearValue clearBlock = { };
          clearBlock.depthStencil.depth = float(depth) / (m_state.depthRange.n - m_state.depthRange.f);
          clearBlock.depthStencil.stencil = 0;

          Rc<DxvkImageView> view = textureDepth->GetView(0);
          VkImageSubresourceRange subresources = textureDepth->GetImage()->getAvailableSubresources();

          EmitCs([
            cView = view,
            cSubResource = subresources,
            cClearValue = clearBlock
          ](DxvkContext* ctx) {
            DxvkAttachment attachment = {};
            attachment.view = cView;
            VkImageAspectFlags cAspectMask = cSubResource.aspectMask;
            ctx->clearRenderTarget(attachment, cAspectMask, cClearValue, 0u);
          });
          Logger::debug(str::format("depth ", m_state.renderBuffer, " cleared"));
        }
      }
    }
  }

  void GlideDevice::BufferSwap(int swapInterval) {
    GlideDeviceLock lock = LockDevice();

    if (m_swapchain == nullptr)
      return;

    m_swapchain->BufferSwap(true, swapInterval);
    m_dirty.set(GlideDeviceDirtyFlag::Framebuffer);
  }

  void GlideDevice::ApplyTopology(glide3x::GrDrawVertexArrayMode_t mode) {
    if (m_iaState.mode != mode) {
      m_iaState.mode = mode;

      EmitCs([
        cTopology = UtilVertexModeToTopology(mode)
      ] (DxvkContext* ctx) {
        auto iaState = DxvkInputAssemblyState(cTopology, false);
        ctx->setInputAssemblyState(iaState);
      });
    }
  }

  void GlideDevice::BindBlend() {
    m_dirty.clr(GlideDeviceDirtyFlag::Blend);

    DxvkBlendMode mode = { };
    mode.setColorOp(UtilDecodeBlendFactor(m_state.blend.srcColor, false, true),
                    UtilDecodeBlendFactor(m_state.blend.dstColor, false, false),
                    VK_BLEND_OP_ADD);

    mode.setAlphaOp(UtilDecodeBlendFactor(m_state.blend.srcAlpha, true, true),
                    UtilDecodeBlendFactor(m_state.blend.dstAlpha, true, false),
                    VK_BLEND_OP_ADD);

    VkColorComponentFlags writeMask = 0;
    if (m_state.colorMask.color)
      writeMask |= VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT;
    if (m_state.colorMask.alpha)
      writeMask |= VK_COLOR_COMPONENT_A_BIT;

    if (likely(!m_glideOptions.hackNoBlend)) {
      mode.setBlendEnable(true);
    } else {
      mode.setBlendEnable(false);
    }

    EmitCs([this,
      cMode = mode,
      cWriteMask = writeMask
    ] (DxvkContext* ctx) {
      for (uint32_t i = 0; i < 4; i++) {
        DxvkBlendMode mode = cMode;
        mode.setWriteMask(cWriteMask);
        mode.normalize();

        mode.setColorOp(mode.colorSrcFactor(),
                        mode.colorDstFactor(), mode.colorBlendOp());
        mode.setAlphaOp(mode.alphaSrcFactor(),
                        mode.alphaDstFactor(), mode.alphaBlendOp());

        ctx->setBlendMode(i, mode);
      }
    });
  }

  void GlideDevice::BindFramebuffer() {
    m_dirty.clr(GlideDeviceDirtyFlag::Framebuffer);

    Rc<GlideCommonTexture> textureRT = nullptr;
    if (!m_swapchain->GetBuffer(m_state.renderBuffer, textureRT))
      return;

    VkImageAspectFlags feedbackLoopAspects = VK_IMAGE_ASPECT_COLOR_BIT;
    DxvkRenderTargets attachments;
    attachments.color[0].view = textureRT->GetView(0);

    if (likely(!m_glideOptions.hackNoDepth)) {
      Rc<GlideCommonTexture> textureDepth = nullptr;
      if (m_swapchain->GetBuffer(GR_BUFFER_AUXBUFFER, textureDepth)) {
        feedbackLoopAspects |= VK_IMAGE_ASPECT_DEPTH_BIT;
        attachments.depth.view = textureDepth->GetView(0);
      }
    }

    EmitCs([
      cAttachments         = std::move(attachments),
      cFeedbackLoopAspects = feedbackLoopAspects
    ](DxvkContext* ctx) mutable {
      ctx->bindRenderTargets(std::move(cAttachments), cFeedbackLoopAspects);
    });

    ConsiderFlush(GpuFlushType::ImplicitWeakHint);
  }

  void GlideDevice::BindRasterizerState() {
    m_dirty.clr(GlideDeviceDirtyFlag::RasterizerState);

    GlideDeviceLock lock = LockDevice();

    DxvkRasterizerState rsState;
    if (unlikely(m_state.originLocation == GrOriginLocation_t::GR_ORIGIN_LOWER_LEFT)) {
      rsState.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    } else {
      rsState.setFrontFace(VK_FRONT_FACE_CLOCKWISE);
    }

    rsState.setSampleCount(VkSampleCountFlags(VK_SAMPLE_COUNT_1_BIT));
    rsState.setCullMode(UtilCullModeVKCullModeFlags(m_state.cullMode));
    rsState.setConservativeMode(VK_CONSERVATIVE_RASTERIZATION_MODE_DISABLED_EXT);
    rsState.setLineMode(VK_LINE_RASTERIZATION_MODE_DEFAULT);
    rsState.setPolygonMode(VK_POLYGON_MODE_FILL);
    rsState.setFlatShading(false);

    EmitCs([
      cState    = rsState
    ](DxvkContext* ctx) {
      ctx->setRasterizerState(cState);
    });
  }

  void GlideDevice::BindSampler(int tmu) {
    m_samplerBindCount++;

    const auto& TMU = m_tmu[tmu];
    const GlideTMU::TMUTexture* texture = TMU->GetTexture(m_state.textures[tmu]);
    if (unlikely(texture == nullptr || texture->texture == nullptr))
      return;

    Rc<GlideCommonTexture> tex = texture->texture;
    Rc<DxvkImageView> imageView = tex->GetView(0);
    size_t mipmaps = imageView->imageSubresources().levelCount;

    EmitCs([this,
      cTMUState   = m_tmu[tmu]->m_state,
      cMipMaps    = mipmaps,
      cSlot       = tmu,
      cView       = std::move(imageView),
      cBindId     = m_samplerBindCount
    ] (DxvkContext* ctx) {
      DxvkSamplerKey key = { };

      if (cMipMaps > 1)
        key.setLodRange(0.0, cMipMaps, cTMUState.texLodBiasValue);
      else
        key.setLodRange(0.0f, 1.0f, cTMUState.texLodBiasValue);
      key.setLodRange(0.0f, 1.0f, cTMUState.texLodBiasValue);
      key.setUsePixelCoordinates(false);
      key.setFilter(cTMUState.texFilterMode.min == GR_TEXTUREFILTER_POINT_SAMPLED ? VK_FILTER_NEAREST : VK_FILTER_LINEAR,
                    cTMUState.texFilterMode.mag == GR_TEXTUREFILTER_POINT_SAMPLED ? VK_FILTER_NEAREST : VK_FILTER_LINEAR,
                    cTMUState.texMipMapMode.mode != GR_MIPMAP_DISABLE && cTMUState.texMipMapMode.lodBlend ? VK_SAMPLER_MIPMAP_MODE_LINEAR
                                                                                                          : VK_SAMPLER_MIPMAP_MODE_NEAREST);
      key.setAddressModes(
        UtilGlideClampToVKSamplerAddressMode(cTMUState.texClampMode.s),
        UtilGlideClampToVKSamplerAddressMode(cTMUState.texClampMode.t),
        VK_SAMPLER_ADDRESS_MODE_REPEAT);

      key.setDepthCompare(true, VK_COMPARE_OP_LESS_OR_EQUAL);

      if (cView)
        key.setViewProperties(cView->info().unpackSwizzle(), cView->info().format);

      auto [stage, slot] = getTextureSlotInfo(cSlot);
      ctx->bindResourceSampler(stage, slot, m_device->createSampler(key));

      // Let the main thread know about current sampler stats
      uint64_t liveCount = m_device->getSamplerStats().liveCount;
      m_lastSamplerStats.store(liveCount | (cBindId << SamplerCountBits));
    });
  }

  void GlideDevice::BindSpecConstants() {
    m_dirty.clr(GlideDeviceDirtyFlag::SpecializationEntries);

    EmitCs([
      cSpecData = m_specData
    ] (DxvkContext* ctx) {
      ctx->setSpecConstants(VK_PIPELINE_BIND_POINT_GRAPHICS, 0u, sizeof(cSpecData) / sizeof(uint32_t), &cSpecData);
    });
  }

  void GlideDevice::BindVertexLayout() {
    m_dirty.clr(GlideDeviceDirtyFlag::VertexLayout);

    size_t attrCount = m_state.vertexLayout.GetCount();
    std::array<DxvkVertexInput, GLIDE_MAX_VERTEX_PARAM_COUNT> attrList = m_state.vertexLayout.GetAttributeList();
    DxvkVertexBinding binding = {};

    const size_t bindCount = 1;
    std::array<DxvkVertexInput, bindCount> bindList = {};
    binding.divisor = 0u;
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindList[0] = DxvkVertexInput(binding);

    EmitCs([
      cAttrCount   = attrCount,
      cBindCount   = bindCount,
      cAttrList    = std::move(attrList),
      cBindList    = std::move(bindList)
    ](DxvkContext* ctx) {
      ctx->setInputLayout(
          cAttrCount, cAttrList.data(),
          cBindCount, cBindList.data());
    });

    m_dirty.set(GlideDeviceDirtyFlag::FFShader);
  }

  void GlideDevice::BindViewportAndScissor() {
    m_dirty.clr(GlideDeviceDirtyFlag::Viewport);

    GlideDeviceLock lock = LockDevice();

    constexpr float cf = 0.0f;
    float minZ = 0.0f, maxZ = 1.0f;
    float zBias = 0.0f;

    DxvkViewport state = { };
    const bool inverted = m_state.originLocation == GrOriginLocation_t::GR_ORIGIN_LOWER_LEFT;
    const VkRect2D& viewport = m_state.viewport;
    state.viewport = VkViewport{
      float(viewport.offset.x) + cf, float((inverted ? viewport.extent.height : 0.0) + viewport.offset.y) + cf,
      float(viewport.extent.width), (inverted ? -1.0f : 1.0f) * float(viewport.extent.height),
      std::clamp(minZ, 0.0f, 1.0f),
      std::clamp(std::max(maxZ, minZ + zBias), 0.0f, 1.0f),
    };
    state.scissor = m_state.scissor;

    EmitCs([
      cViewport = std::move(state)
    ](DxvkContext* ctx) {
      ctx->setViewports(1, &cViewport);
    });
  }

  void GlideDevice::EmitFeedbackLoopBarriers(FxBool rt, FxBool depth) {
    struct {
      FxBool RT : 1;
      FxBool DS : 1;
    } hazardState;
    hazardState.RT = rt;
    hazardState.DS = depth;

    EmitCs([
      cHazardState = hazardState
    ](DxvkContext* ctx) {
      VkPipelineStageFlags srcStages = 0;
      VkAccessFlags srcAccess = 0;

      if (cHazardState.RT != 0) {
        srcStages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        srcAccess |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      }
      if (cHazardState.DS != 0) {
        srcStages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        srcAccess |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      }

      ctx->emitGraphicsBarrier(
        srcStages,
        srcAccess,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_ACCESS_SHADER_READ_BIT);
    });
  }

  void GlideDevice::PrepareDraw(glide3x::GrDrawVertexArrayMode_t primitiveTopology) {
//    EmitFeedbackLoopBarriers();

    for (int tmu = 0; tmu < std::min(m_boardConfiguration.tmuCount, GLIDE_MAX_TEXTURESTAGES); tmu++) {
      const auto& TMU = m_tmu[tmu];
      if (TMU->UploadTextures(m_state.textures[tmu])) {
        const GlideTMU::TMUTexture* texture = TMU->GetTexture(m_state.textures[tmu]);
        if (unlikely(texture == nullptr || texture->texture == nullptr)) {
          Logger::debug(str::format("Texture on TMU ", tmu, " not found!"));
          continue;
        }

        EmitCs<false>([
          cSlot       = tmu,
          cImageView  = texture->texture->GetView(0)
        ] (DxvkContext* ctx) mutable {
          auto [stage, slot] = getTextureSlotInfo(cSlot);
          ctx->bindResourceImageView(stage, slot, std::move(cImageView));
        });

        BindSampler(tmu);

        if (m_pushData.ffps.aspectRatioLog2[tmu] != texture->aspectRatioLog2) {
          m_pushData.ffps.aspectRatioLog2[tmu] = texture->aspectRatioLog2;
          m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
        }
      } else {
        Logger::debug(str::format("Texture on TMU ", tmu, " not uploaded!"));
      }

      ConsiderFlush(GpuFlushType::ImplicitWeakHint);
    }

    ApplyTopology(primitiveTopology);

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::VertexLayout)))
      BindVertexLayout();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::RasterizerState)))
      BindRasterizerState();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::Viewport)))
      BindViewportAndScissor();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::Blend)))
      BindBlend();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::Framebuffer)))
      BindFramebuffer();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::SpecializationEntries)))
      BindSpecConstants();

    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::Depth)))
      UpdateDepth();

    UpdateFixedFunction();
  }

  void GlideDevice::UpdateDepth() {
    m_dirty.clr(GlideDeviceDirtyFlag::Depth);

    if (unlikely(m_glideOptions.hackNoDepth)) {
      return;
    }

    float depthBias            = bit::cast<float>(m_state.depthBiasLevel);

    DxvkDepthBias biases;
    biases.depthBiasConstant = depthBias;
    biases.depthBiasClamp    = 0.0f;

    DxvkDepthBounds bounds;
    bounds.minDepthBounds = float(m_state.depthRange.f) / (m_state.depthRange.n - m_state.depthRange.f);
    bounds.maxDepthBounds = float(m_state.depthRange.n) / (m_state.depthRange.n - m_state.depthRange.f);

    DxvkDepthStencilState state = { };
    state.setDepthTest(m_auxBuffers > 0 && m_state.depthMode != GR_DEPTHBUFFER_DISABLE);
    state.setDepthWrite(m_auxBuffers > 0 && m_state.depthMode != GR_DEPTHBUFFER_DISABLE && m_state.depthMask);
    state.setStencilTest(false);
    state.setDepthCompareOp(UtilCompareModeToVKCompareMode(m_state.depthFunction));
    state.normalize();

    EmitCs([
      cBiases = biases,
      cBounds = bounds,
      cState  = state
    ] (DxvkContext* ctx) {
      ctx->setDepthBias(cBiases);
      ctx->setDepthBounds(cBounds);
      ctx->setDepthStencilState(cState);
    });
  }

  void GlideDevice::UpdateFixedFunction() {
    if (unlikely(m_dirty.test(GlideDeviceDirtyFlag::FFShader))) {
      DxvkMultisampleState msState = { };
      msState.setSampleMask(uint16_t(0xffffu));
      msState.setAlphaToCoverage(false);

      EmitCs([
        cState          = msState,
        cFS             = m_shaders.GetShader<GlideShaderType::PixelShader>()
      ] (DxvkContext* ctx) mutable {
        ctx->bindShader<VK_SHADER_STAGE_FRAGMENT_BIT>(std::move(cFS));
        ctx->setMultisampleState(cState);
      });
    }

    UpdatePushData();
  }

  template<typename T>
  void GlideDevice::UpdatePushDataBlock(const T& Block) {
    EmitCs([cBlock = Block] (DxvkContext* ctx) {
      ctx->pushData(T::Stages, T::Offset, sizeof(T), &cBlock);
    });
  }

  void GlideDevice::UpdatePushData() {
    if (m_dirty.test(GlideDeviceDirtyFlag::PushDataShared)) {
      if (m_api == GLIDEAPI::API_GLIDE_3X) {
        m_pushData.shared.flags = 0;
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Q0))
          m_pushData.shared.flags |= FXBIT(1);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_ST0))
          m_pushData.shared.flags |= FXBIT(2);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Q1))
          m_pushData.shared.flags |= FXBIT(3);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_ST1))
          m_pushData.shared.flags |= FXBIT(4);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Q2))
          m_pushData.shared.flags |= FXBIT(5);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_ST2))
          m_pushData.shared.flags |= FXBIT(6);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_RGB))
          m_pushData.shared.flags |= FXBIT(7);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_W))
          m_pushData.shared.flags |= FXBIT(8);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Q))
          m_pushData.shared.flags |= FXBIT(9);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Z))
          m_pushData.shared.flags |= FXBIT(10);

        // signal that it is Glide3X with emulated hints
        m_pushData.shared.flags |= FXBIT(31);
      } else {
        m_pushData.shared.flags = m_state.stwhints;
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_RGB))
          m_pushData.shared.flags |= FXBIT(7);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_W))
          m_pushData.shared.flags |= FXBIT(8);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Q))
          m_pushData.shared.flags |= FXBIT(9);
        if (m_state.vertexLayout.TestParam(glide3x::GR_PARAM_Z))
          m_pushData.shared.flags |= FXBIT(10);
      }

      if (m_glideOptions.ignoreGammaCorrection)
        m_pushData.shared.flags |= FXBIT(11);

      m_pushData.shared.flags |= m_state.colorFormat << 30;
      m_pushData.shared.viewport = m_state.viewport;
      m_pushData.shared.coordinateSpace = m_state.coordinateSpace;

      UpdatePushDataBlock(m_pushData.shared);
    }
    if (m_dirty.test(GlideDeviceDirtyFlag::PushDataVs))
      UpdatePushDataBlock(m_pushData.vs);
    if (m_dirty.test(GlideDeviceDirtyFlag::PushDataFfvs))
      UpdatePushDataBlock(m_pushData.ffvs);
    if (m_dirty.test(GlideDeviceDirtyFlag::PushDataFfps)) {
      for (int idx = 0; idx < std::min(m_boardConfiguration.tmuCount, GLIDE_MAX_TEXTURESTAGES); idx++) {
        const Rc<GlideTMU>& tmu = m_tmu[idx];

        uint32_t& combinerTMU = m_pushData.ffps.combinerTMU[idx];
        FxI32 fnc = tmu->m_state.texCombine.alphaFunction;
        // There is 0x0A - 0x0F hole, so rebind last 0x10 to 0x0A
        if (tmu->m_state.texCombine.alphaFunction == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA)
          fnc = 0x0A;
        combinerTMU = (fnc & 0x0F);
        // Similarly rebind last 0x10 to 0x0A
        fnc = tmu->m_state.texCombine.colorFunction;
        if (tmu->m_state.texCombine.colorFunction == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA)
          fnc = 0x0A;
        combinerTMU |= (fnc & 0x0F) << 4;
        combinerTMU |= (tmu->m_state.texCombine.alphaFactor & 0x0F) << 8;
        combinerTMU |= (tmu->m_state.texCombine.colorFactor & 0x0F) << 12;
        combinerTMU |= (tmu->m_state.texCombine.alphaInvert & 0x01) << 16;
        combinerTMU |= (tmu->m_state.texCombine.colorInvert & 0x01) << 17;
        combinerTMU |= (tmu->m_state.texClampMode.s & 0x02) << 18;
        combinerTMU |= (tmu->m_state.texClampMode.t & 0x02) << 20;
      }

      Logger::debug(str::format("pixelFxCombiner pushData: "
                                 , "alphaCombine.function - ", m_state.alphaCombine.function, ", "
                                 , "colorCombine.function - ", m_state.colorCombine.function, ", "
                                 , "alphaCombine.factor - ", m_state.alphaCombine.factor, ", "
                                 , "colorCombine.factor - ", m_state.colorCombine.factor, ", "
                                 , "alphaCombine.local - ", m_state.alphaCombine.local, ", "
                                 , "colorCombine.local - ", m_state.colorCombine.local, ", "
                                 , "alphaCombine.other - ", m_state.alphaCombine.other, ", "
                                 , "colorCombine.other - ", m_state.colorCombine.other, ", "
                                 , "alphaCombine.invert - ", m_state.alphaCombine.invert, ", "
                                 , "colorCombine.invert - ", m_state.colorCombine.invert));

      m_pushData.ffps.combinerPixelFx = (m_state.alphaCombine.function & 0x0F)
                               | ((m_state.colorCombine.function & 0x0F) << 4)
                               | ((m_state.alphaCombine.factor & 0x0F) << 8)
                               | ((m_state.colorCombine.factor & 0x0F) << 12)
                               | ((m_state.alphaCombine.local & 0x03) << 16)
                               | ((m_state.colorCombine.local & 0x03) << 18)
                               | ((m_state.alphaCombine.other & 0x03) << 20)
                               | ((m_state.colorCombine.other & 0x03) << 22)
                               | ((m_state.alphaCombine.invert & 0x01) << 24)
                               | ((m_state.colorCombine.invert & 0x01) << 25);
      m_pushData.ffps.alphaTestFunction = m_state.alphaTestFunction;
      m_pushData.ffps.alphaTestReference = m_state.alphaTestReference;
      m_pushData.ffps.chromaKeyLow = m_state.chromaKeyLow;
      m_pushData.ffps.chromaKeyHigh = m_state.chromaKeyHigh;
      m_pushData.ffps.constantColor = m_state.constantColor;
      m_pushData.ffps.depthMode = m_state.depthMode;
      m_pushData.ffps.depth[0] = m_state.depthRange.n;
      m_pushData.ffps.depth[1] = m_state.depthRange.f;
      m_pushData.ffps.ditherMode = m_state.ditherMode;
      m_pushData.ffps.gammaCorrection[0] = m_state.gammaCorrection[0];
      m_pushData.ffps.gammaCorrection[1] = m_state.gammaCorrection[1];
      m_pushData.ffps.gammaCorrection[2] = m_state.gammaCorrection[2];
      m_pushData.ffps.stipple = ((m_state.stippleMode & 0xFF) << 8) | (m_state.stipplePattern & 0xFF);

      m_pushData.ffps.flags = 0;
      if (m_state.alphaLighting)
        m_pushData.ffps.flags = FXBIT(0);
      if (m_state.colorMask.color)
        m_pushData.ffps.flags = FXBIT(1);
      if (m_state.colorMask.alpha)
        m_pushData.ffps.flags = FXBIT(2);

      UpdatePushDataBlock(m_pushData.ffps);
    }

    m_dirty.clr(GlideDeviceDirtyFlag::PushDataShared,
                GlideDeviceDirtyFlag::PushDataVs,
                GlideDeviceDirtyFlag::PushDataFfvs,
                GlideDeviceDirtyFlag::PushDataFfps);
  }

  void GlideDevice::Reset() {
    GlideDeviceLock lock = LockDevice();

    m_state.alphaTestFunction = GR_CMP_ALWAYS;
    m_state.alphaLighting = FXFALSE;
    m_state.alphaTestReference = 0.0f;
    m_state.alphaCombine.factor = GR_COMBINE_FACTOR_ONE;
    m_state.alphaCombine.function = GR_COMBINE_FUNCTION_SCALE_OTHER;
    m_state.alphaCombine.local = GR_COMBINE_LOCAL_NONE;
    m_state.alphaCombine.other = GR_COMBINE_OTHER_CONSTANT;
    m_state.alphaCombine.invert = FXFALSE;
    m_state.blend.dstAlpha = GR_BLEND_ZERO;
    m_state.blend.dstColor = GR_BLEND_ZERO;
    m_state.blend.srcAlpha = GR_BLEND_ONE;
    m_state.blend.srcColor = GR_BLEND_ONE;
    m_state.colorCombine.factor = GR_COMBINE_FACTOR_ONE;
    m_state.colorCombine.function = GR_COMBINE_FUNCTION_SCALE_OTHER;
    m_state.colorCombine.local = GR_COMBINE_LOCAL_ITERATED;
    m_state.colorCombine.other = GR_COMBINE_OTHER_ITERATED;
    m_state.colorCombine.invert = false;
    m_state.colorMask.alpha = FXFALSE;
    m_state.colorMask.color = FXTRUE;
    m_state.chromaKeyLow = 0;
    m_state.chromaKeyHigh = 0;
    m_specData.chromaMode = GR_CHROMAKEY_DISABLE;
    m_state.colorFormat = GR_COLORFORMAT_ARGB;
    m_state.constantColor = 0xFFFFFFFF;
    m_state.coordinateSpace = glide3x::GR_WINDOW_COORDS;
    m_state.cullMode = GR_CULL_DISABLE;
    m_state.depthBiasLevel = 0;
    m_state.depthFunction = GR_CMP_LESS;
    m_state.depthMask = FXTRUE;
    m_state.depthMode = GR_DEPTHBUFFER_DISABLE;
    m_state.depthRange.n = SST1_ZDEPTHVALUE_NEAREST;
    m_state.depthRange.f = SST1_ZDEPTHVALUE_FARTHEST;
    m_state.ditherMode = GR_DITHER_DISABLE;
    m_state.gammaCorrection[0] = 0.0f;
    m_state.gammaCorrection[1] = 0.0f;
    m_state.gammaCorrection[2] = 0.0f;
    m_state.originLocation = GR_ORIGIN_UPPER_LEFT;
    m_state.renderBuffer = GR_BUFFER_BACKBUFFER;
    m_state.scissor = {};
    m_state.stippleMode = glide3x::GR_STIPPLE_DISABLE;
    m_state.stipplePattern = 0;
    m_state.stwhints = 0;
    m_state.textures = {};
    m_state.viewport = {};
    m_state.vertexLayout = {};

    for (int idx = 0; idx < m_boardConfiguration.tmuCount; idx++) {
      const auto& TMU = m_tmu[idx];
      if (idx < GLIDE_MAX_TEXTURESTAGES)
        m_state.textures[idx].startAddress = GLIDE_NO_TEXTURE;
      TMU->m_state.texClampMode.s = GR_TEXTURECLAMP_CLAMP;
      TMU->m_state.texClampMode.t = GR_TEXTURECLAMP_CLAMP;
      TMU->m_state.texCombine.alphaFactor = GR_COMBINE_FACTOR_ONE;
      TMU->m_state.texCombine.alphaFunction = GR_COMBINE_FUNCTION_LOCAL;
      TMU->m_state.texCombine.alphaInvert = FXFALSE;
      TMU->m_state.texCombine.colorFactor = GR_COMBINE_FACTOR_ONE;
      TMU->m_state.texCombine.colorFunction = GR_COMBINE_FUNCTION_LOCAL;
      TMU->m_state.texCombine.colorInvert = FXFALSE;
      TMU->m_state.texDetailControl.lodBias = 0;
      TMU->m_state.texDetailControl.scale = 1;
      TMU->m_state.texDetailControl.max = 1.0f;
      TMU->m_state.texFilterMode.min = GR_TEXTUREFILTER_POINT_SAMPLED;
      TMU->m_state.texFilterMode.mag = GR_TEXTUREFILTER_POINT_SAMPLED;
      TMU->m_state.texLodBiasValue = 0.0f;
      TMU->m_state.texMipMapMode.lodBlend = FXFALSE;
      TMU->m_state.texMipMapMode.mode = GR_MIPMAP_DISABLE;
    }

    m_dirty.set(
      GlideDeviceDirtyFlag::Blend,
      GlideDeviceDirtyFlag::Depth,
      GlideDeviceDirtyFlag::Framebuffer,
      GlideDeviceDirtyFlag::FFShader,
      GlideDeviceDirtyFlag::PushDataShared,
      GlideDeviceDirtyFlag::PushDataVs,
      GlideDeviceDirtyFlag::PushDataFfvs,
      GlideDeviceDirtyFlag::PushDataFfps,
      GlideDeviceDirtyFlag::RasterizerState,
      GlideDeviceDirtyFlag::SpecializationEntries,
      GlideDeviceDirtyFlag::VertexLayout,
      GlideDeviceDirtyFlag::Viewport
    );
  }

  void GlideDevice::BeginFrame(Rc<DxvkLatencyTracker> LatencyTracker, uint64_t FrameId) {
    GlideDeviceLock lock = LockDevice();

    EmitCs<false>([
      cTracker = std::move(LatencyTracker),
      cFrameId = FrameId
    ] (DxvkContext* ctx) {
      if (cTracker && cTracker->needsAutoMarkers())
        ctx->beginLatencyTracking(cTracker, cFrameId);
    });
  }

  void GlideDevice::EndFrame(Rc<DxvkLatencyTracker> LatencyTracker) {
    GlideDeviceLock lock = LockDevice();

    EmitCs<false>([
      cTracker = std::move(LatencyTracker)
    ] (DxvkContext* ctx) {
      ctx->endFrame();

      if (cTracker && cTracker->needsAutoMarkers())
        ctx->endLatencyTracking(cTracker);
    });
  }

  void GlideDevice::EmitCsChunk(DxvkCsChunkRef&& chunk) {
    // Flush init commands so that the CS thread
    // can processe them before the first use.
    // m_initializer->FlushCsChunk();

    // Reset last CS command since it is no longer safe to access
    m_csDataType = GlideCmdType::None;
    m_csData = nullptr;

    // Constant buffers may hold a pointer into the current chunk,
    // reset that here so the data won't get overwritten.
    // for (auto& cbv : m_constantBuffers)
    //   cbv.ResetStreamCommand();

    m_csSeqNum = m_csThread.dispatchChunk(std::move(chunk));
  }

  void GlideDevice::InjectCsChunk(
          DxvkCsChunkRef&&            Chunk,
          bool                        Synchronize) {
    m_csThread.injectChunk(DxvkCsQueue::HighPriority, std::move(Chunk), Synchronize);
  }

  void GlideDevice::ConsiderFlush(GpuFlushType FlushType) {
    uint64_t chunkId = GetCurrentSequenceNumber();
    uint64_t submissionId = m_submissionFence->value();

    if (m_flushTracker.considerFlush(FlushType, chunkId, submissionId, 0u))
      ExecuteFlush();
  }

  void GlideDevice::ExecuteFlush() {
    // Update signaled staging buffer counter and signal the fence
    m_stagingMemorySignaled = 0;//GetStagingMemoryStatistics().allocatedTotal;

    // Reset counter for discarded memory in flight
    m_discardMemoryOnFlush = m_discardMemoryCounter;

    // Add commands to flush the threaded
    // context, then flush the command list
    uint64_t submissionId = ++m_submissionId;

    EmitCs<false>([
      cSubmissionFence  = m_submissionFence,
      cSubmissionId     = submissionId,
      cSubmissionStatus = nullptr,
      cStagingBufferFence = m_stagingBufferFence,
      cStagingBufferAllocated = m_stagingMemorySignaled
    ] (DxvkContext* ctx) {
      ctx->signal(cSubmissionFence, cSubmissionId);
      ctx->signal(cStagingBufferFence, cStagingBufferAllocated);
      ctx->flushCommandList(nullptr, cSubmissionStatus);
    });

    FlushCsChunk();

    m_flushSeqNum = m_csSeqNum;
    m_flushTracker.notifyFlush(m_flushSeqNum, submissionId);
  }

  void GlideDevice::Flush() {
    GlideDeviceLock lock = LockDevice();

    ExecuteFlush();
  }

  GpuFlushType GlideDevice::GetMaxFlushType() const {
    return GpuFlushType::ImplicitWeakHint;
  }

  void GlideDevice::SynchronizeCsThread(uint64_t SequenceNumber) {
    GlideDeviceLock lock = LockDevice();

    // Dispatch current chunk so that all commands
    // recorded prior to this function will be run
    if (SequenceNumber > m_csSeqNum)
      FlushCsChunk();

    m_csThread.synchronize(SequenceNumber);
  }


  void GlideDevice::DrawLine(const void *v1, const void *v2) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(v1 == nullptr || v2 == nullptr))
      return;

    constexpr uint32_t vertexCount = 2;
    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();
    const size_t bufferSize = vertexCount * vertexSize;

    auto slice = m_stagingBuffer.alloc(bufferSize);
    void* dst = slice.mapPtr(0);
    std::memcpy(static_cast<uint8_t*>(dst) + 0 * vertexSize, v1, vertexSize);
    std::memcpy(static_cast<uint8_t*>(dst) + 1 * vertexSize, v2, vertexSize);

    PrepareDraw(glide3x::GR_LINES);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = vertexCount,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  void GlideDevice::DrawPoint(const void *pt) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(pt == nullptr))
      return;

    constexpr uint32_t vertexCount = 1;
    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();
    const size_t bufferSize = vertexCount * vertexSize;

    auto slice = m_stagingBuffer.alloc(bufferSize);
    void* dst = slice.mapPtr(0);
    std::memcpy(static_cast<uint8_t*>(dst), pt, vertexSize);

    PrepareDraw(glide3x::GR_POINTS);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = vertexCount,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  void GlideDevice::DrawTriangle(const void *a, const void *b, const void *c) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(a == nullptr || b == nullptr || c == nullptr))
      return;

    constexpr uint32_t vertexCount = 3;
    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();
    const size_t bufferSize = vertexCount * vertexSize;

    auto slice = m_stagingBuffer.alloc(bufferSize);
    void* dst = slice.mapPtr(0);
    std::memcpy(static_cast<uint8_t*>(dst) + 0 * vertexSize, a, vertexSize);
    std::memcpy(static_cast<uint8_t*>(dst) + 1 * vertexSize, b, vertexSize);
    std::memcpy(static_cast<uint8_t*>(dst) + 2 * vertexSize, c, vertexSize);

    PrepareDraw(glide3x::GR_TRIANGLES);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = vertexCount,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  void GlideDevice::DrawVertexArray(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(pointers == nullptr))
      return;

    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();
    const size_t bufferSize = count * vertexSize;
    DxvkBufferSlice slice = m_stagingBuffer.alloc(bufferSize);
    void **pointer_elems = (void**)pointers;

    for (FxU32 i = 0; i < count; i++) {
      uint8_t* dst = static_cast<uint8_t*>(slice.mapPtr(i * vertexSize));
      const uint8_t* src = static_cast<const uint8_t*>(pointer_elems[i]);
      std::memcpy(dst, src, vertexSize);
    }

    PrepareDraw(mode);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = count,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  void GlideDevice::DrawVertexArrayContiguous(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers, FxU32 stride) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(pointers == nullptr))
      return;

    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();

    if (stride == 0)
      stride = vertexSize;

    const size_t bufferSize = count * vertexSize;
    DxvkBufferSlice slice = m_stagingBuffer.alloc(bufferSize);

    for (FxU32 i = 0; i < count; i++) {
      uint8_t* dst = static_cast<uint8_t*>(slice.mapPtr(i * vertexSize));
      const uint8_t* src = static_cast<const uint8_t*>(pointers) + i * stride;
      std::memcpy(dst, src, stride);
    }

    PrepareDraw(mode);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = count,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  void GlideDevice::DrawVertexArrayIndexed(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers, const void *indices, FxU32 stride) {
    GlideDeviceLock lock = LockDevice();

    if (unlikely(pointers == nullptr || indices == nullptr))
      return;

    const size_t vertexSize = m_state.vertexLayout.GetVertexSize();

    if (stride == 0)
      stride = vertexSize;

    const size_t bufferSize = count * vertexSize;
    DxvkBufferSlice slice = m_stagingBuffer.alloc(bufferSize);

    if (stride == 0)
      stride = vertexSize;

    for (FxU32 i = 0; i < count; i++) {
      uint8_t* dst = static_cast<uint8_t*>(slice.mapPtr(i * vertexSize));
      const uint8_t* src = static_cast<const uint8_t*>(pointers) + static_cast<const int32_t*>(indices)[i] * stride;
      std::memcpy(dst, src, vertexSize);
    }

    PrepareDraw(mode);

    EmitCs([
      cBufferSlice = std::move(slice),
      cCount       = count,
      cStride      = vertexSize,
      cFS          = m_shaders.GetShader<GlideShaderType::PixelShader>(),
      cVS          = m_shaders.GetShader<GlideShaderType::VertexShader>()
    ](DxvkContext* ctx) mutable {
      VkDrawIndirectCommand draw = { };
      draw.vertexCount = cCount;
      draw.instanceCount = 1u;

      ctx->bindVertexBuffer(0, std::move(cBufferSlice), cStride);
      ctx->draw(1u, &draw);
      ctx->bindVertexBuffer(0, DxvkBufferSlice(), 0);
    });
  }

  GrColorFormat_t GlideDevice::GetColorFormat() const {
    return m_state.colorFormat;
  }

  FxI32 GlideDevice::QueryResolutions(const glide3x::GrResolution *resTemplate, glide3x::GrResolution *output) {
    GlideDeviceLock lock = LockDevice();

    Logger::debug("Reporting display modes request");
    if (unlikely(resTemplate == nullptr))
      return 0;

    HMONITOR monitor = wsi::getWindowMonitor(m_window);
    if (unlikely(!monitor))
      return 0;

    wsi::WsiMode mode;
    FxI32 size = 0;
    int count = 0;
    const bool extendedResolution = resTemplate->resolution == glide3x::GR_EXTENDED;
    const bool extendedRefreshRate = resTemplate->refresh == glide3x::GR_EXTENDED;
    Logger::debug(str::format("Reporting display modes extended status: ", extendedResolution, "@", extendedResolution));
    for (DWORD idx = 0; wsi::getDisplayMode(monitor, idx, &mode); idx++) {
      // let's hope everyone is using monitors with 32bit color support by now
      if (mode.bitsPerPixel != 32)
        continue;

      const size_t modeRefresh = mode.refreshRate.numerator / mode.refreshRate.denominator;

      GrScreenResolution_t resolution;
      if (!UtilDimensionsToResolution(mode.width, mode.height, resolution, extendedResolution))
        continue;

      GrScreenRefresh_t refreshRate;
      if (!UtilNumberToRefreshRate(modeRefresh, refreshRate, extendedRefreshRate))
        continue;

      if (resTemplate->resolution != glide3x::GR_QUERY_ANY && resTemplate->resolution != glide3x::GR_EXTENDED && resTemplate->resolution != resolution)
        continue;

      if (resTemplate->refresh != glide3x::GR_QUERY_ANY && resTemplate->refresh != glide3x::GR_EXTENDED && resTemplate->refresh != refreshRate)
        continue;

      if (output != nullptr) {
        glide3x::GrResolution* curMode = output + count;

        curMode->resolution = resolution;
        curMode->numAuxBuffers = GLIDE_MAX_AUX_BUFFERS;
        curMode->numColorBuffers = GLIDE_MAX_SWAP_CHAIN_BUFFERS;
        curMode->refresh = refreshRate;
        Logger::debug(str::format("Reporting display mode: ", mode.width, "x", mode.height, "@", modeRefresh, " (", resolution, "@", refreshRate, ")"));
      }

      count++;
      size += sizeof(glide3x::GrResolution);
    }

    Logger::debug(str::format("Reporting display modes done: ", size));
    return size;
  }
  
  void GlideDevice::SetAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.blend.dstAlpha != alpha_df || m_state.blend.dstColor != rgb_df ||
        m_state.blend.srcAlpha != alpha_sf || m_state.blend.srcColor != rgb_sf) {
      m_state.blend.dstAlpha = alpha_df;
      m_state.blend.dstColor = rgb_df;
      m_state.blend.srcAlpha = alpha_sf;
      m_state.blend.srcColor = rgb_sf;
      m_dirty.set(GlideDeviceDirtyFlag::Blend);
    }
  }

  void GlideDevice::SetAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.alphaCombine.function != function || m_state.alphaCombine.factor != factor ||
        m_state.alphaCombine.local != local || m_state.alphaCombine.other != other || m_state.alphaCombine.invert != invert) {
      m_state.alphaCombine.function = function;
      m_state.alphaCombine.factor = factor;
      m_state.alphaCombine.local = local;
      m_state.alphaCombine.other = other;
      m_state.alphaCombine.invert = invert;
      Logger::debug(str::format("pixelFxCombiner setAlphaCombine: "
                                  , "alphaCombine.function - ", m_state.alphaCombine.function, ", "
                                  , "alphaCombine.factor - ", m_state.alphaCombine.factor, ", "
                                  , "alphaCombine.local - ", m_state.alphaCombine.local, ", "
                                  , "alphaCombine.other - ", m_state.alphaCombine.other, ", "
                                  , "alphaCombine.invert - ", m_state.alphaCombine.invert));

      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetAlphaSource(GrAlphaSource_t mode) {
    GrCombineFunction_t function;
    GrCombineFactor_t factor;
    GrCombineLocal_t local;
    GrCombineOther_t other;
    FxBool invert = false;

    switch (mode) {
      case GR_ALPHASOURCE_CC_ALPHA:
        function = GR_COMBINE_FUNCTION_LOCAL;
        factor = GR_COMBINE_FACTOR_NONE;
        local = GR_COMBINE_LOCAL_CONSTANT;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXFALSE;
        break;
      case GR_ALPHASOURCE_ITERATED_ALPHA:
        function = GR_COMBINE_FUNCTION_LOCAL;
        factor = GR_COMBINE_FACTOR_NONE;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXFALSE;
        break;
      case GR_ALPHASOURCE_TEXTURE_ALPHA:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_ONE;
        local = GR_COMBINE_LOCAL_NONE;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_ITERATED_ALPHA:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_LOCAL;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      default:
        return;
    }

    SetAlphaCombine(function, factor, local, other, invert);
  }

  void GlideDevice::SetAlphaLighting(FxBool enable) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.alphaLighting != enable) {
      m_state.alphaLighting = enable;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetAlphaTestFunction(GrCmpFnc_t function) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.alphaTestFunction != function) {
      m_state.alphaTestFunction = function;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetAlphaTestReferenceValue(GrAlpha_t value) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.alphaTestReference != value) {
      m_state.alphaTestReference = value;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetChromaKeyMode(GrChromakeyMode_t mode) {
    GlideDeviceLock lock = LockDevice();

    if (m_specData.chromaMode != static_cast<uint32_t>(mode)) {
      m_specData.chromaMode = mode;
      m_dirty.set(GlideDeviceDirtyFlag::SpecializationEntries);
    }
  }

  void GlideDevice::SetChromaKeyValue(GrColor_t value) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.chromaKeyLow != value || m_state.chromaKeyHigh != value) {
      m_state.chromaKeyLow = value;
      m_state.chromaKeyHigh = value;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.colorCombine.function != function || m_state.colorCombine.factor != factor || m_state.colorCombine.local != local || m_state.colorCombine.other != other || m_state.colorCombine.invert != invert) {
      m_state.colorCombine.function = function;
      m_state.colorCombine.factor = factor;
      m_state.colorCombine.local = local;
      m_state.colorCombine.other = other;
      m_state.colorCombine.invert = invert;
      Logger::debug(str::format("pixelFxCombiner setColorCombine: "
                                  , "colorCombine.function - ", function, ", "
                                  , "colorCombine.factor - ", factor, ", "
                                  , "colorCombine.local - ", local, ", "
                                  , "colorCombine.other - ", other, ", "
                                  , "colorCombine.invert - ", invert));

      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetColorCombineFunction(GrColorCombineFnc_t fnc){
    GrCombineFunction_t function;
    GrCombineFactor_t factor;
    GrCombineLocal_t local;
    GrCombineOther_t other;
    FxBool invert = false;

    switch (fnc) {
      case GR_COLORCOMBINE_ZERO:
        function = GR_COMBINE_FUNCTION_ZERO;
        factor = GR_COMBINE_FACTOR_NONE;
        local = GR_COMBINE_LOCAL_NONE;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_CCRGB:
        function = GR_COMBINE_FUNCTION_LOCAL,
        factor = GR_COMBINE_FACTOR_ZERO;
        local = GR_COMBINE_LOCAL_CONSTANT;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_ITRGB_DELTA0: {
        // TODO: proper handling of DELTA0 state
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn(str::format("GR_COLORCOMBINE_ITRGB_DELTA0 unsupported!"));
      }
        // fall-through
      case GR_COLORCOMBINE_ITRGB:
        function = GR_COMBINE_FUNCTION_LOCAL;
        factor = GR_COMBINE_FACTOR_NONE;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_DECAL_TEXTURE:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_ONE;
        local = GR_COMBINE_LOCAL_NONE;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_LOCAL;
        local = GR_COMBINE_LOCAL_CONSTANT;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0: {
        // TODO: proper handling of DELTA0 state
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn(str::format("GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0 unsupported!"));
      }
        // fall-through
      case GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_LOCAL;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_ADD_ALPHA:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA;
        factor = GR_COMBINE_FACTOR_LOCAL;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        factor = GR_COMBINE_FACTOR_LOCAL_ALPHA;
        local = GR_COMBINE_LOCAL_NONE;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA_ADD_ITRGB:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL;
        factor = GR_COMBINE_FACTOR_LOCAL_ALPHA;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
      break;
      case GR_COLORCOMBINE_TEXTURE_ADD_ITRGB:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL;
        factor = GR_COMBINE_FACTOR_ONE;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_TEXTURE_SUB_ITRGB:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL;
        factor = GR_COMBINE_FACTOR_ONE;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_CCRGB_BLEND_ITRGB_ON_TEXALPHA:
        function = GR_COMBINE_FUNCTION_BLEND;
        factor = GR_COMBINE_FACTOR_TEXTURE_ALPHA;
        local = GR_COMBINE_LOCAL_CONSTANT;
        other = GR_COMBINE_OTHER_ITERATED;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_DIFF_SPEC_A:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL;
        factor = GR_COMBINE_FACTOR_LOCAL_ALPHA;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_DIFF_SPEC_B:
        function = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA;
        factor = GR_COMBINE_FACTOR_LOCAL;
        local = GR_COMBINE_LOCAL_ITERATED;
        other = GR_COMBINE_OTHER_TEXTURE;
        invert = FXFALSE;
        break;
      case GR_COLORCOMBINE_ONE:
        function = GR_COMBINE_FUNCTION_ZERO;
        factor = GR_COMBINE_FACTOR_NONE;
        local = GR_COMBINE_LOCAL_NONE;
        other = GR_COMBINE_OTHER_NONE;
        invert = FXTRUE;
        break;
      default:
        Logger::err(str::format("Unknown color combine function: ", fnc));
        return;
    }

    Logger::debug(str::format("pixelFxCombiner setColorCombineFunction: "
                                 , "function - ", fnc, ", "
                                 , "colorCombine.function - ", function, ", "
                                 , "colorCombine.factor - ", factor, ", "
                                 , "colorCombine.local - ", local, ", "
                                 , "colorCombine.other - ", other, ", "
                                 , "colorCombine.invert - ", invert));


    SetColorCombine(function, factor, local, other, invert);
  }

  void GlideDevice::SetColorMask(FxBool rgb, FxBool a) {
    if (m_state.colorMask.color != rgb || m_state.colorMask.alpha != a) {
      m_state.colorMask.color = rgb;
      m_state.colorMask.alpha = a;
      m_dirty.set(GlideDeviceDirtyFlag::Blend, GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetColorFormat(GrColorFormat_t color) {
    if (m_state.colorFormat != color) {
      m_state.colorFormat = color;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataShared);
    }
  }

  void GlideDevice::SetConstantColor(GrColor_t color) {
    if (m_state.constantColor != color) {
      m_state.constantColor = color;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetCullMode(GrCullMode_t mode) {
    if (m_state.cullMode != mode) {
      m_state.cullMode = mode;
      m_dirty.set(GlideDeviceDirtyFlag::RasterizerState);
    }
  }

  void GlideDevice::SetDepthBiasLevel(FxI32 level) {
    if (m_state.depthBiasLevel != level) {
      m_state.depthBiasLevel = level;
      m_dirty.set(GlideDeviceDirtyFlag::Depth);
    }
  }

  void GlideDevice::SetDepthBufferFunction(GrCmpFnc_t function) {
    if (m_state.depthFunction != function) {
      m_state.depthFunction = function;
      m_dirty.set(GlideDeviceDirtyFlag::Depth);
    }
  }

  void GlideDevice::SetDepthBufferMode(GrDepthBufferMode_t mode) {
    if (m_state.depthMode != mode) {
      m_state.depthMode = mode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
      m_dirty.set(GlideDeviceDirtyFlag::Depth);
    }
  }

  void GlideDevice::SetDepthMask(FxBool mask) {
    if (m_state.depthMask != mask) {
      m_state.depthMask = mask;
      m_dirty.set(GlideDeviceDirtyFlag::Depth);
    }
  }

  void GlideDevice::SetDepthRange(FxFloat n, FxFloat f) {
    if (m_state.depthRange.n != n || m_state.depthRange.f != f) {
      m_state.depthRange.n = n;
      m_state.depthRange.f = f;
      m_dirty.set(GlideDeviceDirtyFlag::Depth);
    }
  }

  void GlideDevice::SetDitherMode(GrDitherMode_t mode) {
    if (m_state.ditherMode != mode) {
      m_state.ditherMode = mode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetGammaCorrection(FxFloat red, FxFloat green, FxFloat blue) {
    if (m_state.gammaCorrection[0] != red || m_state.gammaCorrection[1] != green || m_state.gammaCorrection[2] != blue || m_state.gammaCorrection[0] != red) {
      m_state.gammaCorrection[0] = red;
      m_state.gammaCorrection[1] = green;
      m_state.gammaCorrection[2] = blue;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetHints(GrHint_t hintType, GrSTWHint_t hintMask) {
    switch (hintType) {
      case GR_HINT_STWHINT:
        if (m_state.stwhints != hintMask) {
          m_state.stwhints = hintMask;
          m_dirty.set(GlideDeviceDirtyFlag::PushDataShared);
        }
        break;
      default:
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn(str::format("Unhandled hint: ", hintType));
    }
  }

  void GlideDevice::SetRenderBuffer(GrBuffer_t buffer) {
    if (unlikely(buffer != GR_BUFFER_FRONTBUFFER && buffer != GR_BUFFER_AUXBUFFER)) {
      static bool sWarnShown = false;
      if (!std::exchange(sWarnShown, true))
        Logger::warn(str::format("Unexpected render buffer: ", buffer));

      return;
    }

    // TODO: aux can be used not only for depth!

    if (m_state.renderBuffer != buffer) {
      m_state.renderBuffer = buffer;
      m_dirty.set(GlideDeviceDirtyFlag::Framebuffer);
    }
  }

  void GlideDevice::SetStippleMode(glide3x::GrStippleMode_t mode) {
    if (m_state.stippleMode != mode) {
      m_state.stippleMode = mode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetStipplePattern(glide3x::GrStipplePattern_t pattern) {
    m_state.stipplePattern = pattern & 0xFF;
    m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
  }

  void GlideDevice::SetTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texCombine.colorFunction != rgb_function || TMU->m_state.texCombine.colorFactor != rgb_factor ||
        TMU->m_state.texCombine.colorInvert != rgb_invert || TMU->m_state.texCombine.alphaFunction != alpha_function ||
        TMU->m_state.texCombine.alphaFactor != alpha_factor || TMU->m_state.texCombine.alphaInvert != alpha_invert) {
      TMU->m_state.texCombine.colorFunction = rgb_function;
      TMU->m_state.texCombine.colorFactor = rgb_factor;
      TMU->m_state.texCombine.colorInvert = rgb_invert;
      TMU->m_state.texCombine.alphaFunction = alpha_function;
      TMU->m_state.texCombine.alphaFactor = alpha_factor;
      TMU->m_state.texCombine.alphaInvert = alpha_invert;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GrCombineFunction_t alphaFunction, colorFunction;
    GrCombineFactor_t alphaFactor, colorFactor;
    FxBool alphaInvert, colorInvert;

    switch (fnc)  {
      case GR_TEXTURECOMBINE_ZERO:
        colorFunction = GR_COMBINE_FUNCTION_ZERO;
        colorFactor = GR_COMBINE_FACTOR_NONE;
        alphaFunction = GR_COMBINE_FUNCTION_ZERO;
        alphaFactor = GR_COMBINE_FACTOR_NONE;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_DECAL:
        colorFunction = GR_COMBINE_FUNCTION_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_NONE;
        alphaFunction = GR_COMBINE_FUNCTION_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_NONE;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_ONE:
        colorFunction = GR_COMBINE_FUNCTION_ZERO;
        colorFactor = GR_COMBINE_FACTOR_NONE;
        alphaFunction = GR_COMBINE_FUNCTION_ZERO;
        alphaFactor = GR_COMBINE_FACTOR_NONE;
        colorInvert = FXTRUE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_ADD:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_ONE;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_ONE;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_MULTIPLY:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER;
        colorFactor = GR_COMBINE_FACTOR_LOCAL;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER;
        alphaFactor = GR_COMBINE_FACTOR_LOCAL;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_DETAIL:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_DETAIL_OTHER:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_DETAIL_FACTOR;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_DETAIL_FACTOR;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_TRILINEAR_ODD:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_TRILINEAR_EVEN:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_LOD_FRACTION;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_LOD_FRACTION;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_SUBTRACT:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL;
        colorFactor = GR_COMBINE_FACTOR_ONE;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL;
        alphaFactor = GR_COMBINE_FACTOR_ONE;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      case GR_TEXTURECOMBINE_OTHER:
        colorFunction = GR_COMBINE_FUNCTION_SCALE_OTHER;
        colorFactor = GR_COMBINE_FACTOR_ONE;
        alphaFunction = GR_COMBINE_FUNCTION_SCALE_OTHER;
        alphaFactor = GR_COMBINE_FACTOR_ONE;
        colorInvert = FXFALSE;
        alphaInvert = FXFALSE;
        break;
      default:
        return;
    }

    if (TMU->m_state.texCombine.alphaFactor != alphaFactor || TMU->m_state.texCombine.alphaFunction != alphaFunction ||
        TMU->m_state.texCombine.alphaInvert != alphaInvert || TMU->m_state.texCombine.colorFactor != colorFactor ||
        TMU->m_state.texCombine.colorFunction != colorFunction || TMU->m_state.texCombine.colorInvert != colorInvert) {
      TMU->m_state.texCombine.alphaFactor = alphaFactor;
      TMU->m_state.texCombine.alphaFunction = alphaFunction;
      TMU->m_state.texCombine.alphaInvert = alphaInvert;
      TMU->m_state.texCombine.colorFactor = colorFactor;
      TMU->m_state.texCombine.colorFunction = colorFunction;
      TMU->m_state.texCombine.colorInvert = colorInvert;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexClampMode(GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texClampMode.s != s_clampmode || TMU->m_state.texClampMode.t != t_clampmode) {
      TMU->m_state.texClampMode.s = s_clampmode;
      TMU->m_state.texClampMode.t = t_clampmode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, FxFloat detail_max) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texDetailControl.lodBias != lod_bias || TMU->m_state.texDetailControl.scale != detail_scale || TMU->m_state.texDetailControl.max != detail_max) {
      TMU->m_state.texDetailControl.lodBias = lod_bias;
      TMU->m_state.texDetailControl.scale = detail_scale;
      TMU->m_state.texDetailControl.max = detail_max;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexFilterMode(GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texFilterMode.min != minfilter_mode || TMU->m_state.texFilterMode.mag != magfilter_mode) {
      TMU->m_state.texFilterMode.min = minfilter_mode;
      TMU->m_state.texFilterMode.mag = magfilter_mode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexLodBiasValue(GrChipID_t tmu, FxFloat bias) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texLodBiasValue != bias) {
      TMU->m_state.texLodBiasValue = bias;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexMipMapMode(GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend) {
    GlideDeviceLock lock = LockDevice();

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    if (TMU->m_state.texMipMapMode.mode != mode || TMU->m_state.texMipMapMode.lodBlend != lodBlend) {
      TMU->m_state.texMipMapMode.mode = mode;
      TMU->m_state.texMipMapMode.lodBlend = lodBlend;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataFfps);
    }
  }

  void GlideDevice::SetTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd
    , glide3x::GrLOD_t smallLodLog2, glide3x::GrLOD_t largeLodLog2
    , glide3x::GrAspectRatio_t aspectRatioLog2
    , GrTextureFormat_t format) {
    GlideDeviceLock lock = LockDevice();
    Logger::debug(str::format("Set GPU texture request at TMU ", tmu, " with format ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(format)), " address: ", startAddress));

    if (tmu >= GLIDE_MAX_TEXTURESTAGES)
      return;

    Rc<GlideTMU> TMU = GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    m_state.textures[tmu].startAddress = startAddress;
    m_state.textures[tmu].smallLodLog2 = smallLodLog2;
    m_state.textures[tmu].largeLodLog2 = largeLodLog2;
    m_state.textures[tmu].smallLodLog2 = smallLodLog2;
    m_state.textures[tmu].aspectRatioLog2 = aspectRatioLog2;
    m_state.textures[tmu].format = format;
    m_state.textures[tmu].evenOdd = evenOdd;

    Logger::debug(str::format("Set GPU texture at TMU ", tmu, " done with format ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(format)), " address: ", startAddress));
  }

  void GlideDevice::SetViewport(FxI32 x, FxI32 y, FxU32 width, FxU32 height) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.viewport.offset.x != x || m_state.viewport.offset.y != y ||
        m_state.viewport.extent.width != width || m_state.viewport.extent.height != height) {
      m_state.viewport.offset.x = x;
      m_state.viewport.offset.y = y;
      m_state.viewport.extent.width = width;
      m_state.viewport.extent.height = height;

      m_dirty.set(GlideDeviceDirtyFlag::Viewport, GlideDeviceDirtyFlag::PushDataShared);
    }
  }

  void GlideDevice::SetCoordinateSpace(glide3x::GrCoordinateSpaceMode_t mode) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.coordinateSpace != mode) {
      m_state.coordinateSpace = mode;
      m_dirty.set(GlideDeviceDirtyFlag::PushDataShared);
    }
  }

  void GlideDevice::SetClipWindow(FxI32 minx, FxI32 miny, FxU32 maxx, FxU32 maxy) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.scissor.offset.x != minx || m_state.scissor.offset.y != miny ||
        m_state.scissor.extent.width != maxx || m_state.scissor.extent.height != maxy) {
      m_state.scissor.offset.x = minx;
      m_state.scissor.offset.y = miny;
      m_state.scissor.extent.width = maxx;
      m_state.scissor.extent.height = maxy;
    }
    m_dirty.set(GlideDeviceDirtyFlag::Viewport);
  }

  void GlideDevice::SetOriginLocation(GrOriginLocation_t origin) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.originLocation != origin) {
      m_state.originLocation = origin;
      m_dirty.set(GlideDeviceDirtyFlag::RasterizerState, GlideDeviceDirtyFlag::Viewport);
    }
  }

  void GlideDevice::SetVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutMode_t mode, glide3x::GrVertexLayoutOffset_t offset) {
    GlideDeviceLock lock = LockDevice();

    if (m_state.vertexLayout.SetVertexLayout(param, mode, offset))
      m_dirty.set(GlideDeviceDirtyFlag::VertexLayout, GlideDeviceDirtyFlag::PushDataShared);
  }

  FxU32 GlideDevice::GetVertexLayoutSize() const {
    return m_state.vertexLayout.GetLayoutSize();
  }

  void GlideDevice::LoadVertexLayout(void *layout) {
    GlideDeviceLock lock = LockDevice();

    if (layout == nullptr)
      return;

    uint32_t id = ++m_vertexLayoutLastID;
    *static_cast<uint32_t*>(layout) = id;
    m_state.vertexLayout.GetLayout(&m_vertexLayouts[id]);
    m_dirty.set(GlideDeviceDirtyFlag::VertexLayout, GlideDeviceDirtyFlag::PushDataShared);
  }

  void GlideDevice::SaveVertexLayout(const void *layout) {
    GlideDeviceLock lock = LockDevice();

    if (layout == nullptr)
      return;

    uint32_t id = (static_cast<const uint32_t *>(layout))[0];
    auto it = m_vertexLayouts.find(id);
    if (it != m_vertexLayouts.end()) {
      m_state.vertexLayout.GetLayout(&it->second);
    }
  }

  void GlideDevice::LoadState(void *state) {
    GlideDeviceLock lock = LockDevice();

    if (state == nullptr)
      return;

    static bool sWarnShown = false;
    if (!std::exchange(sWarnShown, true))
      Logger::warn(str::format("LoadState not implemented!"));
  }

  void GlideDevice::SaveState(const void *state) {
    GlideDeviceLock lock = LockDevice();

    if (state == nullptr)
      return;

    static bool sWarnShown = false;
    if (!std::exchange(sWarnShown, true))
      Logger::warn(str::format("SaveState not implemented!"));
  }

  Rc<GlideTMU> GlideDevice::GetTMU(GrChipID_t tmu) const {
    if (tmu > m_boardConfiguration.tmuCount)
      return nullptr;

    return m_tmu[tmu];
  }

  FxFloat GlideDevice::UtilFogTableIndexToW(FxI32 i) {
      i = std::clamp(i, 0, FOG_TABLE_ENTRIES_COUNT);

      return glideFogIndexToW[i];
  }

  void GlideDevice::UtilFogGenerateLinear(GrFog_t* table, FxFloat nearZ, FxFloat farZ) {
    if (unlikely(!table))
      return;

    int begin, end;
    for (begin = 0; begin < FOG_TABLE_ENTRIES_COUNT; begin++) {
      if (glideFogIndexToW[begin] >= nearZ)
        break;
    }

    for (end = 0; end < FOG_TABLE_ENTRIES_COUNT; end++) {
      if (glideFogIndexToW[end] >= farZ)
        break;
    }

    memset(table, 0x00, sizeof(GrFog_t) * (1 + begin));
    memset(&table[end], 0xFF, sizeof(GrFog_t) * (FOG_TABLE_ENTRIES_COUNT - end));

    for (int i = begin; i <= end; i++) {
      FxFloat f = 255.0f * (FxFloat)(i - begin) / (FxFloat)(end - begin);
      if (f > 255.0f)
        f = 255.0f;
      table[i] = (GrFog_t)f;
    }
  }

  void GlideDevice::UtilFogGenerateExp(GrFog_t* fogTable, FxFloat density) {
    if (unlikely(!fogTable))
      return;

    FxFloat de = density * glideFogIndexToW[FOG_TABLE_ENTRIES_COUNT - 1];
    FxFloat scale = 255.0f / (1.0f - (FxFloat)exp(-de));

    for (int i = 0; i < 64; i++) {
      FxFloat di = density * glideFogIndexToW[i];
      FxFloat r  = (1.0f - (FxFloat)exp(-di)) * scale;
      if (r > 255.0f)
        r = 255.0f;
      else if (r < 0.0f)
        r = 0.0f;

      fogTable[i] = (GrFog_t)r;
    }
  }

  void GlideDevice::UtilFogGenerateExp2(GrFog_t* fogTable, FxFloat density) {
    if (unlikely(!fogTable))
      return;

    for (int i = 0; i < FOG_TABLE_ENTRIES_COUNT; i++) {
      FxFloat edi  = (FxFloat)exp(-density * glideFogIndexToW[i]);
      FxFloat r  = (1.0f - edi * edi) * 255.0f;
      if (r > 255.0f)
        r = 255.0f;
      else if (r < 0.0f)
        r = 0.0f;

      fogTable[i] = (GrFog_t)r;
    }
  }

  FxU32 GlideDevice::GetTMUCount() const {
    return m_boardConfiguration.tmuCount;
  }

  FxU32 GlideDevice::GetTMUMemory() const {
    return m_boardConfiguration.tmuCount * m_boardConfiguration.tmuRam;
  }

  FxU32 GlideDevice::GetTMUMinAddress(GrChipID_t tmu) const {
    if (tmu >= m_boardConfiguration.tmuCount)
      return 0;

    return m_tmu[tmu]->GetMinimumAddress();
  }

  FxU32 GlideDevice::GetTMUMaxAddress(GrChipID_t tmu) const {
    if (tmu >= m_boardConfiguration.tmuCount)
      return 0;

    return m_tmu[tmu]->GetMaximumAddress() - 16;
  }

  void GlideDevice::GetViewportDimensions(FxU32* x, FxU32* y, FxU32* width, FxU32* height) {
    if (unlikely(x == nullptr || y == nullptr || width == nullptr || height == nullptr))
      return;

    *x = m_state.viewport.offset.x;
    *y = m_state.viewport.offset.y;
    *width = m_state.viewport.extent.width;
    *height = m_state.viewport.extent.height;
  }

  void GlideDevice::GetBackBufferDimensions(FxU32* width, FxU32* height) {
    if (likely(width != nullptr))
      *width = m_state.viewport.extent.width;

    if (likely(height != nullptr))
      *height = m_state.viewport.extent.height;
  }
}
