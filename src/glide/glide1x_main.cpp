#include "glide_include.h"

#include "glide_device.h"
#include "glide_options.h"
#include "glide_utils.h"

#include "../util/util_singleton.h"

namespace dxvk {
  Logger Logger::s_instance("glide1x.log");
  static GlideDevice *glideDevice = nullptr;
  constexpr char GlideVersion[80] = "2.11";

  struct {
    FxBool enabled;
    GrLfbBypassMode_t bypassMode;
    GrAlpha_t alpha;
    FxU16 depth;
    GrOriginLocation_t origin;
    GrColorFormat_t colorFormat;
    GrLfbWriteMode_t writeMode;
  } LFB;

  static inline void initDXVKDevice () {
    if (glideDevice != nullptr)
      return;

    try {
      glideDevice = new GlideDevice(GLIDEAPI::API_GLIDE_1X);
    } catch (const DxvkError& e) {
      Logger::err(e.message());
      exit(0);
    }
  }
}

extern "C" {
  using namespace dxvk;
  using namespace dxvk;
  DLLEXPORT void __stdcall ConvertAndDownloadRle(GrChipID_t tmu, FxU32 startAddress
    , glide2x::GrLOD_t thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t aspectRatio
    , GrTextureFormat_t format, MipMapLevelMask_t evenOdd, FxU8 *bm_data, long bm_h, FxU32 u0, FxU32 v0, FxU32 width, FxU32 height, FxU32 dest_width, FxU32 dest_height, FxU16 *tlut) {
    DEBUGLOG("stub: ConvertAndDownloadRle");
  }

  DLLEXPORT void __stdcall grAADrawLine(const glide2x::GrVertex *v1, const glide2x::GrVertex *v2) {
    DEBUGLOG(">>> grAADrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grAADrawPoint(const glide2x::GrVertex *pt) {
    DEBUGLOG(">>> grAADrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grAADrawPolygon(const int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grAADrawPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grAADrawPolygonVertexList(const int nverts, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grAADrawPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grAADrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias) {
    DEBUGLOG(">>> grAADrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df) {
    DEBUGLOG(">>> grAlphaBlendFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaBlendFunction(rgb_sf, rgb_df, alpha_sf, alpha_df);
  }

  DLLEXPORT void __stdcall grAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    DEBUGLOG(">>> grAlphaCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaCombine(function, factor, local, other, invert);
  }

  DLLEXPORT void __stdcall grAlphaControlsITRGBLighting(FxBool enable) {
    DEBUGLOG(">>> grAlphaControlsITRGBLighting");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaLighting(enable);
  }

  DLLEXPORT void __stdcall grAlphaTestFunction(GrCmpFnc_t function) {
    DEBUGLOG(">>> grAlphaTestFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaTestFunction(function);
  }

  DLLEXPORT void __stdcall grAlphaTestReferenceValue(GrAlpha_t value) {
    DEBUGLOG(">>> grAlphaTestReferenceValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaTestReferenceValue(value);
  }

  DLLEXPORT void __stdcall grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU16 depth) {
    DEBUGLOG(">>> grBufferClear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferClear(color, alpha, depth);
  }

  DLLEXPORT int __stdcall grBufferNumPending(void) {
    DEBUGLOG(">>> grBufferNumPending");

    // TODO: report back when buffer finished drawing?

    return 0;
  }

  DLLEXPORT void __stdcall grBufferSwap(int swap_interval) {
    DEBUGLOG(">>> grBufferSwap");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferSwap(swap_interval);
  }

  DLLEXPORT void __stdcall grCheckForRoom(FxI32 n) {
    DEBUGLOG("stub: grCheckForRoom");
  }

  DLLEXPORT void __stdcall grChromakeyMode(GrChromakeyMode_t mode) {
    DEBUGLOG(">>> grChromakeyMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetChromaKeyMode(mode);
  }

  DLLEXPORT void __stdcall grChromakeyValue(GrColor_t value) {
    DEBUGLOG(">>> grChromakeyValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetChromaKeyValue(value);
  }

  DLLEXPORT void __stdcall grClipWindow(FxI32 minx, FxI32 miny, FxI32 maxx, FxI32 maxy) {
    DEBUGLOG(">>> grClipWindow");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetClipWindow(minx, miny, maxx, maxy);
  }

  DLLEXPORT void __stdcall grColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    DEBUGLOG(">>> grColorCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorCombine(function, factor, local, other, invert);
  }

  DLLEXPORT void __stdcall grColorMask(FxBool rgb, FxBool a) {
    DEBUGLOG(">>> grColorMask");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorMask(rgb, a);
  }

  DLLEXPORT void __stdcall grConstantColorValue4(FxFloat a, FxFloat r, FxFloat g, FxFloat b) {
    DEBUGLOG(">>> grConstantColorValue4");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetConstantColor4(a, r, g, b);
  }

  DLLEXPORT void __stdcall grConstantColorValue(GrColor_t value) {
    DEBUGLOG(">>> grConstantColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetConstantColor(value);
  }

  DLLEXPORT void __stdcall grCullMode(GrCullMode_t mode) {
    DEBUGLOG(">>> grCullMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCullMode(mode);
  }

  DLLEXPORT void __stdcall grDepthBiasLevel(FxI16 level) {
    DEBUGLOG(">>> grDepthBiasLevel");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBiasLevel(level);
  }

  DLLEXPORT void __stdcall grDepthBufferFunction(GrCmpFnc_t function) {
    DEBUGLOG(">>> grDepthBufferFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBufferFunction(function);
  }

  DLLEXPORT void __stdcall grDepthBufferMode(GrDepthBufferMode_t mode) {
    DEBUGLOG(">>> grDepthBufferMode");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBufferMode(mode);
  }

  DLLEXPORT void __stdcall grDepthMask(FxBool mask) {
    DEBUGLOG(">>> grDepthMask");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthMask(mask);
  }

  DLLEXPORT void __stdcall grDisableAllEffects(void) {
    DEBUGLOG("stub: grDisableAllEffects");
  }

  DLLEXPORT void __stdcall grDitherMode(GrDitherMode_t mode) {
    DEBUGLOG(">>> grDitherMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDitherMode(mode);
  }

  DLLEXPORT void __stdcall grDrawLine(const glide2x::GrVertex *v1, const glide2x::GrVertex *v2) {
    DEBUGLOG(">>> grDrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grDrawPlanarPolygon(int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grDrawPlanarPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grDrawPlanarPolygonVertexList(int nverts, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grDrawPlanarPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grDrawPoint(const glide2x::GrVertex *pt) {
    DEBUGLOG(">>> grDrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grDrawPolygon(int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grDrawPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grDrawPolygonVertexList(int nverts, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> grDrawPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grDrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    DEBUGLOG(">>> grDrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grErrorSetCallback(GrErrorCallbackFnc_t fnc) {
    DEBUGLOG("stub: grErrorSetCallback");
  }

  DLLEXPORT void __stdcall grFogColorValue(GrColor_t fogColor) {
    DEBUGLOG(">>> grFogColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetFogColor(fogColor);
  }

  DLLEXPORT void __stdcall grFogMode(glide2x::GrFogMode_t mode) {
    DEBUGLOG(">>> grFogMode");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetFogMode(static_cast<glide3x::GrFogMode_t>(mode));
  }

  DLLEXPORT void __stdcall grFogTable(const GrFog_t *ft) {
    DEBUGLOG(">>> grFogTable");

    if (unlikely(glideDevice == nullptr || ft == nullptr))
      return;

    return glideDevice->SetFogTable(ft);
  }

  DLLEXPORT void __stdcall grGammaCorrectionValue(float value) {
    DEBUGLOG(">>> grGammaCorrectionValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetGammaCorrection(value, value, value);
  }

  DLLEXPORT void __stdcall grGlideGetState(glide1x::GrState *state) {
    DEBUGLOG(">>> grGlideGetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->GetState(state);
  }

  DLLEXPORT void __stdcall grGlideGetVersion(char version[80]) {
    DEBUGLOG(">>> grGlideGetVersion");

    if (version == nullptr)
      return;

    std::strncpy(version, GlideVersion, 79);
  }

  DLLEXPORT void __stdcall grGlideInit(void) {
    DEBUGLOG(">>> grGlideInit");

    initDXVKDevice();

    LFB.origin = GR_ORIGIN_UPPER_LEFT;
    LFB.writeMode = GR_LFBWRITEMODE_8888;
    LFB.enabled = FXFALSE;

    glideDevice->SetCoordinateSpace(glide3x::GR_WINDOW_COORDS);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_XY, glide3x::GR_PARAM_ENABLE, 0);
    // 3Dfx headers says that Z is ignored, so not likely to be populated by games
    // glideDevice->SetVertexLayout(glide3x::GR_PARAM_Z, glide3x::GR_PARAM_ENABLE, 8);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_RGB, glide3x::GR_PARAM_ENABLE, 12);
    // ooz
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Z, glide3x::GR_PARAM_ENABLE, 24);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_A, glide3x::GR_PARAM_ENABLE, 28);
    // oow
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_W, glide3x::GR_PARAM_ENABLE, 32);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_ST0, glide3x::GR_PARAM_ENABLE, 36);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Q0, glide3x::GR_PARAM_ENABLE, 44);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_ST1, glide3x::GR_PARAM_ENABLE, 48);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Q1, glide3x::GR_PARAM_ENABLE, 56);
  }

  DLLEXPORT void __stdcall grGlideSetState(const glide1x::GrState *state) {
    DEBUGLOG(">>> grGlideSetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetState(state);
  }

  DLLEXPORT void __stdcall grGlideShamelessPlug(const FxBool on) {
    DEBUGLOG("stub: grGlideShamelessPlug");
  }

  DLLEXPORT void __stdcall grGlideShutdown(void) {
    DEBUGLOG(">>> grGlideShutdown");

    if (unlikely(glideDevice == nullptr))
      return;

    if (glideDevice->GetOptions()->deviceReset) {
      glideDevice->Reset();
      return;
    }

    try {
      delete glideDevice;
      glideDevice = nullptr;
    } catch (const DxvkError& e) {
      Logger::err(str::format("Failed to shutdown device: ", e.message()));
    }
  }

  DLLEXPORT void __stdcall grHints(GrHint_t hintType, GrSTWHint_t hintMask) {
    DEBUGLOG(">>> grHints");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetHints(hintType, hintMask);
  }

  DLLEXPORT void __stdcall grLfbBegin() {
    DEBUGLOG(">>> grLfbBegin");

    LFB.enabled = FXTRUE;
  }

  DLLEXPORT void __stdcall grLfbBypassMode(GrLfbBypassMode_t mode) {
    DEBUGLOG(">>> grLfbBypassMode");

    LFB.bypassMode = mode;
  }

  DLLEXPORT void __stdcall grLfbConstantAlpha(GrAlpha_t alpha) {
    DEBUGLOG(">>> grLfbConstantAlpha");

    LFB.alpha = alpha;
  }

  DLLEXPORT void __stdcall grLfbConstantDepth(FxU16 depth) {
    DEBUGLOG(">>> grLfbConstantDepth");

    LFB.depth = depth;
  }

  DLLEXPORT void __stdcall grLfbEnd() {
    DEBUGLOG(">>> grLfbEnd");

    if (unlikely(glideDevice == nullptr))
      return;

    if (LFB.enabled) {
      LFB.enabled = FXFALSE;
      glideDevice->LinearBufferUnlock(GR_LFB_WRITE_ONLY, GR_BUFFER_BACKBUFFER);
    }
  }

  DLLEXPORT void* __stdcall grLfbGetReadPtr(GrBuffer_t buffer) {
    DEBUGLOG(">>> grLfbGetReadPtr");

    if (unlikely(glideDevice == nullptr || !LFB.enabled))
      return nullptr;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);

    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, buffer, LFB.writeMode, LFB.origin, FXFALSE, &info))
      return nullptr;

    return info.lfbPtr;
  }

  DLLEXPORT void* __stdcall grLfbGetWritePtr(GrBuffer_t buffer) {
    DEBUGLOG(">>> grLfbGetWritePtr");

    if (unlikely(glideDevice == nullptr || !LFB.enabled))
      return nullptr;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);

    if (!glideDevice->LinearBufferLock(GR_LFB_WRITE_ONLY, buffer, LFB.writeMode, LFB.origin, FXFALSE, &info))
      return nullptr;

    return info.lfbPtr;
  }

  DLLEXPORT void __stdcall grLfbOrigin(GrOriginLocation_t origin) {
    DEBUGLOG(">>> grLfbOrigin");

    LFB.origin = origin;
  }

  DLLEXPORT void __stdcall grLfbWriteColorFormat(GrColorFormat_t colorFormat) {
    DEBUGLOG(">>> grLfbWriteColorFormat");

    LFB.colorFormat = colorFormat;
  }

  DLLEXPORT void __stdcall grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords) {
    DEBUGLOG("stub: grLfbWriteColorSwizzle");
  }

  DLLEXPORT void __stdcall grLfbWriteMode(GrLfbWriteMode_t mode) {
    DEBUGLOG(">>> grLfbWriteMode");

    LFB.writeMode = mode;
  }

  // Not in 3Dfx but nGlide has this for extended functionality? Most likely no real users but we'll add it too.
  DLLEXPORT FxI32 __stdcall grQueryResolutions(const glide3x::GrResolution *resTemplate, glide3x::GrResolution *output) {
    DEBUGLOG(">>> grQueryResolutions");
    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->QueryResolutions(resTemplate, output);
  }

  DLLEXPORT void __stdcall grRenderBuffer(GrBuffer_t buffer) {
    DEBUGLOG(">>> grRenderBuffer");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetRenderBuffer(buffer);
  }

  DLLEXPORT void __stdcall grResetTriStats(void) {
    DEBUGLOG("stub: grResetTriStats");
  }

  DLLEXPORT void __stdcall grSplash() {
    DEBUGLOG("stub: grSplash");
  }

  DLLEXPORT void __stdcall grSstConfigPipeline(GrChipID_t chip, GrSstRegister reg, FxU32 value) {
    DEBUGLOG("stub: grSstConfigPipeline");
  }

  DLLEXPORT void __stdcall grSstDetectResources() {
    DEBUGLOG("stub: grSstDetectResources");
  }

  DLLEXPORT void __stdcall grSstIdle(void) {
    DEBUGLOG("stub: grSstIdle");
  }

  DLLEXPORT FxBool __stdcall grSstIsBusy(void) {
    DEBUGLOG("stub: grSstIsBusy");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall grSstOpen(GrScreenResolution_t resolution, GrScreenRefresh_t refreshRate, GrColorFormat_t colorFormat, GrOriginLocation_t originLocation, GrSmoothingMode_t smoothingFilter, FxI32 nColBuffers) {
    DEBUGLOG(">>> grSstWinOpen");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    HWND m_hWnd = GetActiveWindow();

    // TODO: smoothingFilter
    return glideDevice->WindowOpen(m_hWnd, resolution, refreshRate, colorFormat, originLocation, nColBuffers, 0);
  }

  DLLEXPORT void __stdcall grSstOrigin(GrOriginLocation_t origin) {
    DEBUGLOG(">>> grSstOrigin");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetOriginLocation(origin);
  }

  DLLEXPORT void __stdcall grSstPassthruMode(GrPassthruMode_t mode) {
    DEBUGLOG("stub: grSstPassthruMode");
  }

  DLLEXPORT void __stdcall grSstPerfStats(GrSstPerfStats_t *pStats) {
    DEBUGLOG(">>> grSstPerfStats");

    // TODO: support stats?
    if (likely(pStats != nullptr)) {
      pStats->aFuncFail = 0;
      pStats->chromaFail = 0;
      pStats->pixelsIn = 0;
      pStats->pixelsOut = 0;
      pStats->zFuncFail = 0;
    }
  }

  DLLEXPORT FxBool __stdcall grSstQueryBoards(GrHwConfiguration *hwconfig) {
    DEBUGLOG(">>> grSstQueryBoards");

    if (unlikely(hwconfig == nullptr))
      return FXFALSE;

    // Could be called before grGlideInit
    initDXVKDevice();

    memset(hwconfig, 0x00, sizeof(GrHwConfiguration));
    const GrHwConfiguration *hwConfig = glideDevice->GetHWConfiguration();
    hwconfig->num_sst = hwConfig->num_sst;

    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall grSstQueryHardware(GrHwConfiguration *hwconfig) {
    DEBUGLOG(">>> grSstQueryHardware");

    if (unlikely(hwconfig == nullptr))
      return FXFALSE;

    // Could be called before grGlideInit
    initDXVKDevice();

    const GrHwConfiguration *hwConfig = glideDevice->GetHWConfiguration();
    memcpy(hwconfig, hwConfig, sizeof(GrHwConfiguration));

    return FXTRUE;
  }

  DLLEXPORT void __stdcall grSstResetPerfStats(void) {
    DEBUGLOG("stub: grSstResetPerfStats");
  }

  DLLEXPORT FxI32 __stdcall grSstScreenHeight(void) {
    DEBUGLOG(">>> grSstScreenHeight");

    if (unlikely(glideDevice == nullptr))
      return 0;

    FxU32 height;
    glideDevice->GetBackBufferDimensions(nullptr, &height);

    return height;
  }

  DLLEXPORT FxI32 __stdcall grSstScreenWidth(void) {
    DEBUGLOG(">>> grSstScreenWidth");

    if (unlikely(glideDevice == nullptr))
      return 0;

    FxU32 width;
    glideDevice->GetBackBufferDimensions(&width, nullptr);

    return width;
  }

  DLLEXPORT void __stdcall grSstSelect(int sst) {
    DEBUGLOG(">>> grSstSelect");

    // We are emulating a single card
    if (unlikely(sst != 0)) {
      Logger::warn(str::format("Unexpected hwcard requested: ", sst));
    }
  }

  DLLEXPORT FxU32 __stdcall grSstStatus(void) {
    DEBUGLOG(">>> grSstStatus");

    // return PCI/Memory FIFO fully empty and everything not busy
    return 0x0FFFF03F;
  }

  DLLEXPORT FxBool __stdcall grSstVRetraceOn(void) {
    DEBUGLOG("stub: grSstVRetraceOn");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall grSstVidMode(FxU32 whichSst, FxVideoTimingInfo *vidTimings) {
    DEBUGLOG("stub: grSstVidMode");
  }

  DLLEXPORT FxU32 __stdcall grSstVideoLine(void) {
    DEBUGLOG("stub: grSstVideoLine");
    return 0;
  }

  DLLEXPORT FxU32 __stdcall grTexCalcMemRequired(glide2x::GrLOD_t lodmin, glide2x::GrLOD_t lodmax, glide2x::GrAspectRatio_t aspect, GrTextureFormat_t fmt) {
    DEBUGLOG(">>> grTexCalcMemRequired");

    return UtilGetTextureSize(UtilLodToLOG2(lodmin), UtilLodToLOG2(lodmax), UtilAspectRatioToLOG2(aspect), fmt);
  }

  DLLEXPORT void __stdcall grTexClampMode(GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode) {
    DEBUGLOG(">>> grTexClampMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexClampMode(tmu, s_clampmode, t_clampmode);
  }

  DLLEXPORT void __stdcall grTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert) {
    DEBUGLOG(">>> grTexCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombine(tmu, rgb_function, rgb_factor, alpha_function, alpha_factor, rgb_invert, alpha_invert);
  }

  DLLEXPORT void __stdcall grTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc) {
    DEBUGLOG(">>> grTexCombineFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombineFunction(tmu, fnc);
  }

  DLLEXPORT void __stdcall grTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, float detail_max) {
    DEBUGLOG(">>> grTexDetailControl");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexDetailControl(tmu, lod_bias, detail_scale, detail_max);
  }

  DLLEXPORT void __stdcall grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    DEBUGLOG(">>> grTexDownloadMipMap");

    if (unlikely(glideDevice == nullptr))
      return;

    if (unlikely(glideDevice == nullptr || info == nullptr || info->data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = UtilLodToLOG2(info->smallLod);
    m.largeLodLog2 = UtilLodToLOG2(info->largeLod);
    m.aspectRatioLog2 = UtilAspectRatioToLOG2(info->aspectRatio);
    m.format = info->format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(info->data);
    TMU->InsertTextures(std::move(m));
  }

  DLLEXPORT void __stdcall grTexDownloadMipMapLevel(GrChipID_t tmu, FxU32 startAddress, glide2x::GrLOD_t  thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data) {
    DEBUGLOG(">>> grTexDownloadMipMapLevel");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = UtilLodToLOG2(thisLod);
    m.largeLodLog2 = UtilLodToLOG2(largeLod);
    m.aspectRatioLog2 = UtilAspectRatioToLOG2(aspectRatio);
    m.format = format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(data);
    TMU->InsertTextureLevel(std::move(m));
  }

  DLLEXPORT void __stdcall grTexDownloadMipMapLevelPartial(GrChipID_t tmu, FxU32 startAddress, glide2x::GrLOD_t thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data, int start, int end) {
    DEBUGLOG(">>> grTexDownloadMipMapLevelPartial");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = UtilLodToLOG2(thisLod);
    m.largeLodLog2 = UtilLodToLOG2(largeLod);
    m.aspectRatioLog2 = UtilAspectRatioToLOG2(aspectRatio);
    m.format = format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(data);
    TMU->InsertTextureLevel(std::move(m), start, end);
  }

  DLLEXPORT void __stdcall grTexDownloadTable(GrChipID_t tmu, GrTexTable_t type, void *data) {
    DEBUGLOG(">>> grTexDownloadTable");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    if (likely(glideDevice->GetOptions()->perTMUTables)) {
      Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
      if (unlikely(TMU == nullptr))
        return;

      TMU->SetTable(type, data, 0, 0);
    } else {
      Rc<GlideTMU> TMU = glideDevice->GetTMU(GR_TMU0);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, 0, 0);

      TMU = glideDevice->GetTMU(GR_TMU1);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, 0, 0);

      TMU = glideDevice->GetTMU(GR_TMU2);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, 0, 0);
    }
  }

  DLLEXPORT void __stdcall grTexDownloadTablePartial(GrChipID_t tmu, GrTexTable_t type, void *data, int start, int end) {
    DEBUGLOG(">>> grTexDownloadTablePartial");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    if (likely(glideDevice->GetOptions()->perTMUTables)) {
      Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
      if (unlikely(TMU == nullptr))
        return;

      TMU->SetTable(type, data, start, end);
    } else {
      Rc<GlideTMU> TMU = glideDevice->GetTMU(GR_TMU0);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, start, end);

      TMU = glideDevice->GetTMU(GR_TMU1);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, start, end);

      TMU = glideDevice->GetTMU(GR_TMU2);
      if (unlikely(TMU != nullptr))
        TMU->SetTable(type, data, start, end);
    }
  }

  DLLEXPORT void __stdcall grTexFilterMode(GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode) {
    DEBUGLOG(">>> grTexFilterMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexFilterMode(tmu, minfilter_mode, magfilter_mode);
  }

  DLLEXPORT void __stdcall grTexLodBiasValue(GrChipID_t tmu, FxFloat bias) {
    DEBUGLOG(">>> grTexLodBiasValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexLodBiasValue(tmu, bias);
  }

  DLLEXPORT FxU32 __stdcall grTexMaxAddress(GrChipID_t tmu) {
    DEBUGLOG(">>> grTexMaxAddress");

    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->GetTMUMaxAddress(tmu);
  }

  DLLEXPORT FxU32 __stdcall grTexMinAddress(GrChipID_t tmu) {
    DEBUGLOG(">>> grTexMinAddress");

    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->GetTMUMinAddress(tmu);
  }

  DLLEXPORT void __stdcall grTexMipMapMode(GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend) {
    DEBUGLOG(">>> grTexMipMapMode");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetTexMipMapMode(tmu, mode, lodBlend);
  }

  DLLEXPORT void __stdcall grTexMultibase(GrChipID_t tmu, FxBool enable) {
    DEBUGLOG("stub: grTexMultibase");
  }

  DLLEXPORT void __stdcall grTexMultibaseAddress(GrChipID_t tmu, FxU32 range, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    DEBUGLOG("stub: grTexMultibaseAddress");
  }

  DLLEXPORT void __stdcall grTexNCCTable(GrChipID_t tmu, GrTexTable_t table) {
    DEBUGLOG(">>> grTexNCCTable");

    if (table != GR_TEXTABLE_NCC0 && table != GR_TEXTABLE_NCC1)
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    TMU->SelectNCCTable(table);
  }

  DLLEXPORT void __stdcall grTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    DEBUGLOG(">>> grTexSource");

    if (info == nullptr)
      return;

    glideDevice->SetTexSource(tmu, startAddress, evenOdd, UtilLodToLOG2(info->smallLod), UtilLodToLOG2(info->largeLod), UtilAspectRatioToLOG2(info->aspectRatio), info->format);
  }

  DLLEXPORT FxU32 __stdcall grTexTextureMemRequired(MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    DEBUGLOG(">>> grTexTextureMemRequired");

    if (info == nullptr)
      return 0;

    return UtilGetTextureSize(UtilLodToLOG2(info->smallLod), UtilLodToLOG2(info->largeLod), UtilAspectRatioToLOG2(info->aspectRatio), info->format, evenOdd);
  }

  DLLEXPORT void __stdcall grTriStats(FxU32 *trisProcessed, FxU32 *trisDrawn) {
    DEBUGLOG(">>> grTriStats");

    // TODO: support stats?
    if (likely(trisProcessed != nullptr))
      *trisProcessed = 0;

    if (likely(trisDrawn != nullptr))
      *trisDrawn = 0;
  }

  DLLEXPORT FxBool __stdcall gu3dfGetInfo(const char *filename, glide2x::Gu3dfInfo *info) {
    DEBUGLOG("stub: gu3dfGetInfo");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall gu3dfLoad(const char *filename, glide2x::Gu3dfInfo *data) {
    DEBUGLOG("stub: gu3dfLoad");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guAADrawTriangleWithClip(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    DEBUGLOG(">>> guAADrawTriangleWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall guAlphaSource(GrAlphaSource_t mode) {
    DEBUGLOG(">>> guAlphaSource");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaSource(mode);
  }

  DLLEXPORT void __stdcall guColorCombineFunction(GrColorCombineFnc_t fnc) {
  DEBUGLOG(">>> guColorCombineFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorCombineFunction(fnc);
  }

  DLLEXPORT void __stdcall guDrawPolygonVertexListWithClip(int nverts, const glide2x::GrVertex *vlist) {
    DEBUGLOG(">>> guDrawPolygonVertexListWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall guDrawTriangleWithClip(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    DEBUGLOG(">>> guAADrawTriangleWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT int __stdcall guEncodeRLE16(void *dst, void *src, FxU32 width, FxU32 height) {
    DEBUGLOG("stub: guEncodeRLE16");
    return 0;
  }

  DLLEXPORT FxU16 __stdcall guEndianSwapBytes(FxU16 value) {
    DEBUGLOG(">>> guEndianSwapBytes");

    return (value << 8) | (value >> 8);
  }

  DLLEXPORT FxU32 __stdcall guEndianSwapWords(FxU32 value) {
    DEBUGLOG(">>> guEndianSwapWords");

    return (value << 16) | (value >> 16);
  }

  DLLEXPORT void __stdcall guFbReadRegion(const FxI32 srcX, const FxI32 srcY, const FxI32 srcWidth, const FxI32 srcHeight, void *dstData, const FxI32 dstStride) {
    DEBUGLOG(">>> guFbReadRegion");

    if (unlikely(glideDevice == nullptr || dstData == nullptr || dstStride <= 0))
      return;

    GrBuffer_t srcBuffer = glideDevice->GetRenderBuffer();

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    FxU32 blockSize = 2;
    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, srcBuffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return;

    uint8_t* dstBase  = reinterpret_cast<uint8_t*>(dstData);
    uint8_t* srcBase = reinterpret_cast<uint8_t*>(info.lfbPtr);
    for (FxI32 h = 0; h < srcHeight; ++h) {
        uint8_t* srcRow = srcBase + ((srcY + h) * info.strideInBytes) + (srcX * blockSize);
        uint8_t* dstRow = dstBase + (h * dstStride);
        memcpy(dstRow, srcRow, dstStride);
    }

    glideDevice->LinearBufferUnlock(GR_LFB_READ_ONLY, srcBuffer);
  }

  DLLEXPORT void __stdcall guFbWriteRegion(const FxI32 dstX, const FxI32 dstY, const FxI32 srcWidth, const FxI32 srcHeight, void *srcData, const FxI32 srcStride) {
    DEBUGLOG(">>> guFbWriteRegion");

    if (unlikely(glideDevice == nullptr || srcData == nullptr || srcStride <= 0))
      return;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);

    GrIntFmt_t format = GR_FMT_RGB_565;
    GrBuffer_t dstBuffer = glideDevice->GetRenderBuffer();
    if (!glideDevice->LinearBufferLock(GR_LFB_WRITE_ONLY, dstBuffer, UtilIFormatToLFBWriteMode(format), GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return;

    FxU32 blockSize = 0;
    UtilTextureFormatSize(format, &blockSize);
    blockSize >>= 3;
    if (unlikely(blockSize == 0))
      return;

    uint8_t* dstBase = reinterpret_cast<uint8_t*>(info.lfbPtr);
    uint8_t* srcBase  = reinterpret_cast<uint8_t*>(srcData);
    for (FxI32 h = 0; h < srcHeight; ++h) {
        uint8_t* dstRow = dstBase + ((dstY + h) * info.strideInBytes) + (dstX * blockSize);
        uint8_t* srcRow = srcBase + (h * srcStride);
        memcpy(dstRow, srcRow, srcWidth * blockSize);
    }

    glideDevice->LinearBufferUnlock(GR_LFB_WRITE_ONLY, dstBuffer);
  }

  DLLEXPORT void __stdcall guFogGenerateExp(GrFog_t *fogTable, float density) {
    DEBUGLOG(">>> guFogGenerateExp");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateExp2(GrFog_t *fogTable, float density) {
    DEBUGLOG(">>> guFogGenerateExp2");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp2(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateLinear(GrFog_t *fogTable, float nearZ, float farZ) {
    DEBUGLOG(">>> guFogGenerateLinear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateLinear(fogTable, nearZ, farZ);
  }

  DLLEXPORT FxFloat __stdcall guFogTableIndexToW(FxI32 i) {
    DEBUGLOG(">>> guFogTableIndexToW");

    if (unlikely(glideDevice == nullptr))
      return 0.0;

    return glideDevice->UtilFogTableIndexToW(i);
  }

  DLLEXPORT void __stdcall guMPDrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    DEBUGLOG("stub: guMPDrawTriangle");
  }

  DLLEXPORT void __stdcall guMPInit(void) {
    DEBUGLOG("stub: guMPInit");
  }

  DLLEXPORT void __stdcall guMPTexCombineFunction(GrMPTextureCombineFnc_t tc) {
    DEBUGLOG("stub: guMPTexCombineFunction");
  }

  DLLEXPORT void __stdcall guMPTexSource(FxI32 virtual_tmu, GrMipMapId_t mmid) {
    DEBUGLOG("stub: guMPTexSource");
  }

  DLLEXPORT void __stdcall guMovieSetName(const char *name) {
    DEBUGLOG("stub: guMovieSetName");
  }

  DLLEXPORT void __stdcall guMovieStart(void) {
    DEBUGLOG("stub: guMovieStart");
  }

  DLLEXPORT void __stdcall guMovieStop(void) {
    DEBUGLOG("stub: guMovieStop");
  }

  DLLEXPORT GrMipMapId_t __stdcall guTexAllocateMemory(GrChipID_t tmu, FxU8 odd_even_mask, int width, int height, FxI32 fmt, FxI32 mm_mode, FxI32 smallest_lod, FxI32 largest_lod, FxI32 aspect, FxI32 s_clamp_mode, FxI32 t_clamp_mode, FxI32 minfilter_mode, FxI32 magfilter_mode, float lod_bias, FxBool trilinear) {
    DEBUGLOG("stub: guTexAllocateMemory");
    return 0;
  }

  DLLEXPORT FxBool __stdcall guTexChangeAttributes(GrMipMapId_t mmid, int width, int height, FxI32 fmt, FxI32 mm_mode, FxI32 smallest_lod, FxI32 largest_lod, FxI32 aspect, FxI32 s_clamp_mode, FxI32 t_clamp_mode, FxI32 minFilterMode, FxI32 magFilterMode) {
    DEBUGLOG("stub: guTexChangeAttributes");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc) {
    DEBUGLOG(">>> guTexCombineFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombineFunction(tmu, fnc);
  }

  DLLEXPORT FxU16* __stdcall guTexCreateColorMipMap(void) {
    DEBUGLOG("stub: guTexCreateColorMipMap");
    return 0;
  }

  DLLEXPORT void __stdcall guTexDownloadMipMap(GrMipMapId_t mmid, const void *src, const GuNccTable *table) {
    DEBUGLOG("stub: guTexDownloadMipMap");
  }

  DLLEXPORT void __stdcall guTexDownloadMipMapLevel(GrMipMapId_t mmid, FxI32 lod, const void ** src) {
    DEBUGLOG("stub: guTexDownloadMipMapLevel");
  }

  DLLEXPORT GrMipMapId_t __stdcall guTexGetCurrentMipMap(GrChipID_t tmu) {
    DEBUGLOG("stub: guTexGetCurrentMipMap");
    return 0;
  }

  DLLEXPORT glide2x::GrMipMapInfo* __stdcall guTexGetMipMapInfo(GrMipMapId_t mmid) {
    DEBUGLOG("stub: guTexGetMipMapInfo");
    return nullptr;
  }

  DLLEXPORT FxU32 __stdcall guTexMemQueryAvail(GrChipID_t tmu) {
    DEBUGLOG("stub: guTexMemQueryAvail");
    return 0;
  }

  DLLEXPORT void __stdcall guTexMemReset(void) {
    DEBUGLOG("stub: guTexMemReset");
  }

  DLLEXPORT void __stdcall guTexSource(GrMipMapId_t id) {
    DEBUGLOG("stub: guTexSource");
  }

  DLLEXPORT FxBool __stdcall pciClose(void) {
    DEBUGLOG("stub: pciClose");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciDeviceExists(FxU32 device_number) {
    DEBUGLOG("stub: pciDeviceExists");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindCardMulti(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum, FxU32 cardNum) {
    DEBUGLOG("stub: pciFindCardMulti");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum) {
    return pciFindCardMulti(vendorID, deviceID, devNum, 0);
  }

  DLLEXPORT FxBool __stdcall pciFindCardMultiFunc(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum, FxU32 *funcNum, FxU32 functionIndex) {
    DEBUGLOG("stub: pciFindCardMultiFunc");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum) {
    DEBUGLOG("stub: pciFindMTRRMatch");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindFreeMTRR(FxU32 *mtrrNum) {
    DEBUGLOG("stub: pciFindFreeMTRR");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindFreeMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum) {
    DEBUGLOG("stub: pciFindFreeMTRRMatch");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciGetConfigData(PciRegister reg, FxU32 device_number, FxU32 *data) {
    DEBUGLOG("stub: pciGetConfigData");
    return FXFALSE;
  }

  DLLEXPORT FxU32 __stdcall pciGetErrorCode(void) {
    DEBUGLOG("stub: pciGetErrorCode");
    return 0;
  }

  DLLEXPORT const char* __stdcall pciGetErrorString(void) {
    DEBUGLOG("stub: pciGetErrorString");
    return nullptr;
  }

  DLLEXPORT FxU32* __stdcall pciMapCard(FxU32 vID, FxU32 dID, FxI32 len, FxU32 *devNo, FxU32 addrNo) {
    DEBUGLOG("stub: pciMapCard");
    return 0;
  }

  DLLEXPORT FxU32* __stdcall pciMapCardMulti(FxU32 vID,FxU32 dID,FxI32 l,FxU32 *dNo,FxU32 cNo,FxU32 aNo) {
    DEBUGLOG("stub: pciMapCardMulti");
    return 0;
  }

  DLLEXPORT FxBool __stdcall pciMapPhysicalToLinear(unsigned long *linear_addr, FxU32 physical_addr,FxU32 *length) {
    DEBUGLOG("stub: pciMapPhysicalToLinear");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciOpen(void) {
    DEBUGLOG("stub: pciOpen");
    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall pciOpenEx(void) {
    DEBUGLOG("stub: pciOpenEx");
    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall pciOutputDebugString(const char* debugMsg) {
    DEBUGLOG("stub: pciOutputDebugString");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetConfigData(PciRegister reg, FxU32 device_number, FxU32 *data) {
    DEBUGLOG("stub: pciSetConfigData");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetMTRR(FxU32 mtrrNo, FxU32 pBaseAddr, FxU32 psz, PciMemType type) {
    DEBUGLOG("stub: pciSetMTRR");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetPassThroughBase(FxU32* pBaseAddr, FxU32 baseAddrLen) {
    DEBUGLOG("stub: pciSetPassThroughBase");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall pciUnmapPhysical(unsigned long linear_addr, FxU32 length) {
    DEBUGLOG("stub: pciUnmapPhysical");
  }

  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
      case DLL_PROCESS_ATTACH:
        dxvk::Logger::info(">>>>>>> LOADING GXVK >>>>>>>");
        break;
      case DLL_PROCESS_DETACH: {
        dxvk::Logger::info("<<<<<<< UNLOADING GXVK <<<<<<<");
        break;
      }
      default:
        break;
    }
    return FXTRUE;
  }
}
