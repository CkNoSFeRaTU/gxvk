#pragma once

#include "../dxvk/dxvk_instance.h"
#include "../dxvk/dxvk_context_state.h"
#include "../dxvk/dxvk_cs.h"
#include "../dxvk/dxvk_device.h"
#include "../dxvk/dxvk_staging.h"

#include "../util/util_flush.h"

#include "glide_include.h"
#include "glide_multithread.h"
#include "glide_options.h"
#include "glide_shaders.h"
#include "glide_state.h"
#include "glide_texture.h"
#include "glide_swapchain.h"
#include "glide_tmu.h"
#include "glide_vertex_layout.h"

#include <unordered_map>

namespace dxvk {

  enum class GlideCmdType : uint32_t {
    None,
    Draw,
    DrawIndexed,
  };

  struct LFBBuffer {
    GrLfbWriteMode_t format;
    std::vector<uint8_t> buffer;
    FxU32 width;
    FxU32 height;
    FxU32 pixelSize;
  };

  static std::string GlideAPIString(GLIDEAPI api) {
    switch (api) {
      case(GLIDEAPI::API_GLIDE_1X):
        return "Glide1X";
      case(GLIDEAPI::API_GLIDE_2X):
        return "Glide2X";
      case(GLIDEAPI::API_GLIDE_3X):
        return "Glide3X";
    }

    // shouldn't happen
    return "UNKNOWN";
  }

  enum class GlideDeviceDirtyFlag : uint32_t {
    Blend,
    Depth,
    Framebuffer,
    FFShader,
    PushDataShared,
    PushDataVs,
    PushDataFfvs,
    PushDataFfps,
    RasterizerState,
    SpecializationEntries,
    VertexLayout,
    Viewport,
  };

  using GlideDeviceDirtyFlags = Flags<GlideDeviceDirtyFlag>;

  class GlideDevice final {
    constexpr static VkDeviceSize StagingBufferSize = 4ull << 20;
  public:
    GlideDevice(GLIDEAPI api);
    ~GlideDevice();

    std::string GetAPI() const {
      return GlideAPIString(m_api);
    }

    FxBool WindowOpen(HWND hWnd,
                    GrScreenResolution_t resolution,
                    GrScreenRefresh_t refreshRrate,
                    GrColorFormat_t colorFormat,
                    GrOriginLocation_t originLocation,
                    int nColBuffers, int nAuxBuffers);
    void WindowClose();

    const GlideOptions *GetOptions() const {
      return &m_glideOptions;
    };

    const GlideBoardConfiguration* GetBoardConfiguration() const {
      return &m_boardConfiguration;
    }

    const GrHwConfiguration* GetHWConfiguration() const {
      return &m_hwConfiguration;
    }

    FxU32 GlideStateSize() const {
      return sizeof(m_state);
    }

    Rc<GlideTMU> GetTMU(GrChipID_t tmu) const;
    FxU32 GetTMUCount() const;
    FxU32 GetTMUMemory() const;
    FxU32 GetTMUMinAddress(GrChipID_t tmu) const;
    FxU32 GetTMUMaxAddress(GrChipID_t tmu) const;

    void GetViewportDimensions(FxU32* x, FxU32* y, FxU32* width, FxU32* height);
    void GetBackBufferDimensions(FxU32* width, FxU32* height);

    void BufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth);
    void BufferSwap(int swapInterval);
    void DrawLine(const void *v1, const void *v2);
    void DrawPoint(const void *pt);
    void DrawTriangle(const void *a, const void *b, const void *c);
    void DrawVertexArray(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers);
    void DrawVertexArrayContiguous(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers, FxU32 stride = 0);
    void DrawVertexArrayIndexed(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, const void *pointers, const void *indices, FxU32 stride = 0);
    GrColorFormat_t GetColorFormat() const;
    FxBool LinearBufferLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info);
    FxBool LinearBufferUnlock(GrLock_t type, GrBuffer_t buffer);
    void Reset();
    FxI32 QueryResolutions(const glide3x::GrResolution *resTemplate, glide3x::GrResolution *output);
    void SetAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df);
    void SetAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert);
    void SetAlphaLighting(FxBool enable);
    void SetAlphaSource(GrAlphaSource_t mode);
    void SetAlphaTestFunction(GrCmpFnc_t function);
    void SetAlphaTestReferenceValue(GrAlpha_t value);
    void SetChromaKeyMode(GrChromakeyMode_t mode);
    void SetChromaKeyValue(GrColor_t value);
    void SetClipWindow(FxI32 minx, FxI32 miny, FxU32 maxx, FxU32 maxy);
    void SetColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert);
    void SetColorCombineFunction(GrColorCombineFnc_t fnc);
    void SetColorFormat(GrColorFormat_t color);
    void SetColorMask(FxBool rgb, FxBool a);
    void SetConstantColor(GrColor_t color);
    void SetCoordinateSpace(glide3x::GrCoordinateSpaceMode_t mode);
    void SetCullMode(GrCullMode_t mode);
    void SetDepthBiasLevel(FxI32 level);
    void SetDepthBufferFunction(GrCmpFnc_t function);
    void SetDepthBufferMode(GrDepthBufferMode_t mode);
    void SetDepthMask(FxBool mask);
    void SetDepthRange(FxFloat n, FxFloat f);
    void SetDitherMode(GrDitherMode_t mode);
    void SetGammaCorrection(FxFloat red, FxFloat green, FxFloat blue);
    void SetHints(GrHint_t hintType, GrSTWHint_t hintMask);
    void SetOriginLocation(GrOriginLocation_t origin);
    void SetRenderBuffer(GrBuffer_t buffer);
    void SetStippleMode(glide3x::GrStippleMode_t mode);
    void SetStipplePattern(glide3x::GrStipplePattern_t pattern);
    void SetTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert);
    void SetTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc);
    void SetTexClampMode(GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode);
    void SetTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, FxFloat detail_max);
    void SetTexFilterMode(GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode);
    void SetTexLodBiasValue(GrChipID_t tmu, FxFloat bias);
    void SetTexMipMapMode(GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend);
    void SetTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd
      , glide3x::GrLOD_t smallLodLog2, glide3x::GrLOD_t largeLodLog2
      , glide3x::GrAspectRatio_t aspectRatioLog2
      , GrTextureFormat_t format);
    void SetViewport(FxI32 x, FxI32 y, FxU32 width, FxU32 height);

    void SetVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutMode_t mode, glide3x::GrVertexLayoutOffset_t offset);
    FxU32 GetVertexLayoutSize() const;

    void LoadVertexLayout(void *layout);
    void SaveVertexLayout(const void *layout);

    void LoadState(void *state);
    void SaveState(const void *state);

    FxFloat UtilFogTableIndexToW(FxI32 i);
    void UtilFogGenerateLinear(GrFog_t* table, FxFloat nearZ, FxFloat farZ);
    void UtilFogGenerateExp(GrFog_t* fogTable, FxFloat density);
    void UtilFogGenerateExp2(GrFog_t* fogTable, FxFloat density);
      
    DxvkStagingBuffer* GetStagingBuffer() {
      return &m_stagingBuffer;
    }

    inline uint32_t GetUPDataSize(uint32_t vertexCount, uint32_t stride) {
      return vertexCount * stride;
    }

    inline uint32_t GetUPBufferSize(uint32_t vertexCount, uint32_t stride) {
      return (vertexCount - 1) * stride + stride;
    }

    GlideDeviceLock LockDevice() {
      return m_multithread.AcquireLock();
    }

    Rc<DxvkDevice> GetDXVKDevice() const {
      return m_device;
    }

    VkPipelineStageFlags GetEnabledShaderStages() const {
      return m_device->getShaderPipelineStages();
    }

    void BeginFrame(Rc<DxvkLatencyTracker> LatencyTracker, uint64_t FrameId);
    void EndFrame(Rc<DxvkLatencyTracker> LatencyTracker);

    DxvkCsChunkRef AllocCsChunk() {
      DxvkCsChunk* chunk = m_csChunkPool.allocChunk(DxvkCsChunkFlag::SingleUse);
      return DxvkCsChunkRef(chunk, &m_csChunkPool);
    }

    void InjectCsChunk(
            DxvkCsChunkRef&&            Chunk,
            bool                        Synchronize);

    template<typename Fn>
    void InjectCs(
            Fn&&                        Command) {
      auto chunk = AllocCsChunk();
      chunk->push(std::move(Command));

      InjectCsChunk(std::move(chunk), false);
    }

    template<bool AllowFlush = true, typename Cmd>
    void EmitCs(Cmd&& command) {
      if (unlikely(m_csDataType != GlideCmdType::None)) {
        m_csData = nullptr;
        m_csDataType = GlideCmdType::None;
      }

      if (unlikely(!m_csChunk->push(command))) {
        EmitCsChunk(std::move(m_csChunk));
        m_csChunk = AllocCsChunk();

        if constexpr (AllowFlush)
          ConsiderFlush(GpuFlushType::ImplicitWeakHint);

        m_csChunk->push(command);
      }
    }

    template<typename M, bool AllowFlush = true, typename Cmd>
    DxvkCsDataBlock* EmitCsCmd(GlideCmdType type, size_t count, Cmd&& command) {
      m_csDataType = type;
      m_csData = m_csChunk->pushCmd<M, Cmd>(command, count);

      if (unlikely(!m_csData)) {
        EmitCsChunk(std::move(m_csChunk));
        m_csChunk = AllocCsChunk();

        if constexpr (AllowFlush)
          ConsiderFlush(GpuFlushType::ImplicitWeakHint);

        // We must record this command after the potential
        // flush since the caller may still access the data
        m_csData = m_csChunk->pushCmd<M, Cmd>(command, count);
      }

      return m_csData;
    }

    void EmitCsChunk(DxvkCsChunkRef&& chunk);

    void FlushCsChunk() {
      if (likely(!m_csChunk->empty())) {
        EmitCsChunk(std::move(m_csChunk));
        m_csChunk = AllocCsChunk();
      }
    }

    uint64_t GetCurrentSequenceNumber() const {
      return m_csChunk->empty() ? m_csSeqNum : m_csSeqNum + 1;
    }

    void ConsiderFlush(GpuFlushType FlushType);
    void ExecuteFlush();
    void Flush();
    GpuFlushType GetMaxFlushType() const;

    void SynchronizeCsThread(uint64_t SequenceNumber);

  private:
    void ApplyTopology(glide3x::GrDrawVertexArrayMode_t primitiveTopology);
    void UpdateFixedFunction();
    void UpdateDepth();
    void BindBlend();
    void BindFramebuffer();
    void BindSampler(int tmu);
    void BindSpecConstants();
    void BindRasterizerState();
    void BindViewportAndScissor();
    void BindVertexLayout();
    void EmitFeedbackLoopBarriers(FxBool rt, FxBool depth);
    template<typename T>
    void UpdatePushDataBlock(const T& Block);
    void UpdatePushData();
    void PrepareDraw(glide3x::GrDrawVertexArrayMode_t primitiveTopology);

    GLIDEAPI m_api;
    Rc<DxvkInstance>                m_instance;
    Rc<DxvkAdapter>                 m_adapter;
    Rc<DxvkDevice>                  m_device;
    GlideSwapChain*                 m_swapchain = nullptr;
    GlideShaderModuleSet            m_shaders;
    GlideOptions                    m_glideOptions;
    GlideBoardConfiguration         m_boardConfiguration;
    GlidePushData                   m_pushData = {};
    GlideSpecData                   m_specData = {};
    GrHwConfiguration               m_hwConfiguration;
    GlideMultithread                m_multithread;
    GlideDeviceDirtyFlags           m_dirty;
    GlideInputAssemblyState         m_iaState;

    DxvkCsChunkPool                 m_csChunkPool;
    DxvkCsThread                    m_csThread;
    DxvkCsChunkRef                  m_csChunk;
    uint64_t                        m_csSeqNum = 0ull;
    GlideCmdType                    m_csDataType = GlideCmdType::None;
    DxvkCsDataBlock*                m_csData = nullptr;

    DxvkStagingBuffer               m_stagingBuffer;
    Rc<sync::Fence>                 m_stagingBufferFence;
    VkDeviceSize                    m_stagingMemorySignaled = 0ull;
    VkDeviceSize                    m_discardMemoryCounter = 0u;
    VkDeviceSize                    m_discardMemoryOnFlush = 0u;
    Rc<sync::Fence>                 m_submissionFence;
    uint64_t                        m_submissionId = 0ull;
    DxvkSubmitStatus                m_submitStatus;
    uint64_t                        m_flushSeqNum = 0ull;
    GpuFlushTracker                 m_flushTracker;

    DxvkDepthBiasRepresentation     m_depthBiasRepresentation = { VK_DEPTH_BIAS_REPRESENTATION_LEAST_REPRESENTABLE_VALUE_FORMAT_EXT, false };
    float                           m_depthBiasScale  = 0.0f;

    std::atomic<uint32_t> m_refCount = { 0u };

    std::array<
      Rc<GlideTMU>,
      GLIDE_MAX_TMU>                m_tmu = { };
    std::array<LFBBuffer,
      GR_BUFFER_MAXBUFFER>          m_lfb = { };

    FxU32 m_colorBuffers = 0;
    FxU32 m_auxBuffers = 0;

    HWND m_window;
    struct {
      FxU32 width = 0;
      FxU32 height = 0;
      FxU32 refreshRate = 0;
    } m_screen;

    Rc<GlideCommonTexture> m_rt = nullptr;

    struct GlideState {
      GrColor_t constantColor = 0xFFFFFFFF;
      GrCullMode_t cullMode = GR_CULL_DISABLE;
      GrDitherMode_t ditherMode = GR_DITHER_DISABLE;

      std::array<
        GlideTMU::TMUMetadata,
        GLIDE_MAX_TEXTURESTAGES>      textures = {};

      VkRect2D viewport = {};
      VkRect2D scissor = {};
      GrCmpFnc_t alphaTestFunction = GR_CMP_ALWAYS;
      FxU8 alphaTestReference = 0;
      FxFloat gammaCorrection[3] = { 0.0f, 0.0f, 0.0f };
      GlideVertexLayout vertexLayout = {};
      struct {
        GrCombineFunction_t function = GR_COMBINE_FUNCTION_LOCAL;
        GrCombineFactor_t factor = GR_COMBINE_FACTOR_ONE;
        GrCombineLocal_t local = GR_COMBINE_LOCAL_ITERATED;
        GrCombineOther_t other = GR_COMBINE_OTHER_CONSTANT;
        FxBool invert;
      } alphaCombine = {};
      struct {
        GrAlphaBlendFnc_t dstAlpha = GR_BLEND_ONE;
        GrAlphaBlendFnc_t dstColor = GR_BLEND_ZERO;
        GrAlphaBlendFnc_t srcAlpha = GR_BLEND_ONE;
        GrAlphaBlendFnc_t srcColor = GR_BLEND_ZERO;
      } blend;
      struct {
        FxBool color = true;
        FxBool alpha = true;
      } colorMask = {};
      struct {
        GrCombineFunction_t function = GR_COMBINE_FUNCTION_SCALE_OTHER;
        GrCombineFactor_t factor = GR_COMBINE_FACTOR_ONE;
        GrCombineLocal_t local = GR_COMBINE_LOCAL_ITERATED;
        GrCombineOther_t other = GR_COMBINE_OTHER_ITERATED;
        FxBool invert;
      } colorCombine = {};
      GrColor_t chromaKeyLow = 0;
      GrColor_t chromaKeyHigh = 0;
      GrColorFormat_t colorFormat = GR_COLORFORMAT_ARGB;
      glide3x::GrCoordinateSpaceMode_t coordinateSpace = glide3x::GR_WINDOW_COORDS;
      FxI32 depthBiasLevel = 0;
      GrCmpFnc_t depthFunction = GR_CMP_LESS;
      FxBool depthMask = FXTRUE;
      struct {
        FxFloat n;
        FxFloat f;
      } depthRange = {};
      FxBool alphaLighting;
      GrDepthBufferMode_t depthMode = GR_DEPTHBUFFER_DISABLE;
      GrOriginLocation_t originLocation = GR_ORIGIN_UPPER_LEFT;
      GrBuffer_t renderBuffer = GR_BUFFER_BACKBUFFER;
      glide3x::GrStippleMode_t stippleMode = glide3x::GR_STIPPLE_DISABLE;
      glide3x::GrStipplePattern_t stipplePattern = 0;
      uint32_t stwhints = 0;
    } m_state;
    constexpr static uint32_t      SamplerCountBits = 12u;
    std::atomic<uint64_t>          m_lastSamplerStats = { 0u };
    uint64_t                       m_samplerBindCount = 0u;
    std::atomic<uint32_t>          m_vertexLayoutLastID = { 0u };
    std::unordered_map<int, GlideVertex> m_vertexLayouts;
  };

}
