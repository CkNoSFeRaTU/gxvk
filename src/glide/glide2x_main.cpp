#include "glide_include.h"

#include "glide_device.h"
#include "glide_options.h"
#include "glide_utils.h"

#include "../util/util_singleton.h"

namespace dxvk {
  Logger Logger::s_instance("glide2x.log");
  static GlideDevice *glideDevice = nullptr;
  constexpr char GlideVersion[80] = "2.43";

  static inline void initDXVKDevice () {
    if (glideDevice != nullptr)
      return;

    try {
      glideDevice = new GlideDevice(GLIDEAPI::API_GLIDE_2X);
    } catch (const DxvkError& e) {
      Logger::err(e.message());
      exit(0);
    }
  }
}

extern "C" {
  using namespace dxvk;
  DLLEXPORT void __stdcall ConvertAndDownloadRle(GrChipID_t tmu, FxU32 startAddress
    , glide2x::GrLOD_t thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t aspectRatio
    , GrTextureFormat_t format, MipMapLevelMask_t evenOdd, FxU8 *bm_data, long bm_h, FxU32 u0, FxU32 v0, FxU32 width, FxU32 height, FxU32 dest_width, FxU32 dest_height, FxU16 *tlut) {
    Logger::debug("stub: ConvertAndDownloadRle");
  }

  DLLEXPORT void __stdcall grAADrawLine(const glide2x::GrVertex *v1, const glide2x::GrVertex *v2) {
    Logger::debug(">>> grAADrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grAADrawPoint(const glide2x::GrVertex *pt) {
    Logger::debug(">>> grAADrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grAADrawPolygon(const int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grAADrawPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grAADrawPolygonVertexList(const int nverts, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grAADrawPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grAADrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias) {
    Logger::debug(">>> grAADrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df) {
    Logger::debug(">>> grAlphaBlendFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaBlendFunction(rgb_sf, rgb_df, alpha_sf, alpha_df);
  }

  DLLEXPORT void __stdcall grAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    Logger::debug(">>> grAlphaCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaCombine(function, factor, local, other, invert);
  }

  DLLEXPORT void __stdcall grAlphaControlsITRGBLighting(FxBool enable) {
    Logger::debug(">>> grAlphaControlsITRGBLighting");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaLighting(enable);
  }

  DLLEXPORT void __stdcall grAlphaTestFunction(GrCmpFnc_t function) {
    Logger::debug(">>> grAlphaTestFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaTestFunction(function);
  }

  DLLEXPORT void __stdcall grAlphaTestReferenceValue(GrAlpha_t value) {
    Logger::debug(">>> grAlphaTestReferenceValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaTestReferenceValue(value);
  }

  DLLEXPORT void __stdcall grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU16 depth) {
    Logger::debug(">>> grBufferClear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferClear(color, alpha, depth);
  }

  DLLEXPORT int __stdcall grBufferNumPending(void) {
    Logger::debug("stub: grBufferNumPending");

    return 0;
  }

  DLLEXPORT void __stdcall grBufferSwap(int swap_interval) {
    Logger::debug(">>> grBufferSwap");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferSwap(swap_interval);
  }

  DLLEXPORT void __stdcall grCheckForRoom(FxI32 n) {
    Logger::debug("stub: grCheckForRoom");
  }

  DLLEXPORT void __stdcall grChromakeyMode(GrChromakeyMode_t mode) {
    Logger::debug(">>> grChromakeyMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetChromaKeyMode(mode);
  }

  DLLEXPORT void __stdcall grChromakeyValue(GrColor_t value) {
    Logger::debug(">>> grChromakeyValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetChromaKeyValue(value);
  }

  DLLEXPORT void __stdcall grClipWindow(FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy) {
    Logger::debug("stub: grClipWindow");
  }

  DLLEXPORT void __stdcall grColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert) {
    Logger::debug(">>> grColorCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorCombine(function, factor, local, other, invert);
  }

  DLLEXPORT void __stdcall grColorMask(FxBool rgb, FxBool a) {
    Logger::debug(">>> grColorMask");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorMask(rgb, a);
  }

  DLLEXPORT void __stdcall grConstantColorValue4(float a, float r, float g, float b) {
    Logger::debug(">>> grConstantColorValue4");

    if (unlikely(glideDevice == nullptr))
      return;

    FxU8 outA = std::clamp(a, 0.0f, 1.0f) * 255.0f;
    FxU8 outR = std::clamp(r, 0.0f, 1.0f) * 255.0f;
    FxU8 outG = std::clamp(g, 0.0f, 1.0f) * 255.0f;
    FxU8 outB = std::clamp(b, 0.0f, 1.0f) * 255.0f;

    FxU32 color = 0;

    switch (glideDevice->GetColorFormat()) {
      case GrColorFormat_t::GR_COLORFORMAT_ABGR:
        color = (outA << 24) | (outB << 16) | (outG << 8)  | outR;
        break;
      case GrColorFormat_t::GR_COLORFORMAT_ARGB:
        color = (outA << 24) | (outR << 16) | (outG << 8)  | outB;
        break;
      case GrColorFormat_t::GR_COLORFORMAT_BGRA:
        color = (outB << 24) | (outG << 16) | (outR << 8)  | outA;
        break;
      case GrColorFormat_t::GR_COLORFORMAT_RGBA:
        color = (outR << 24) | (outG << 16) | (outB << 8)  | outA;
        break;
    }

    glideDevice->SetConstantColor(color);
  }

  DLLEXPORT void __stdcall grConstantColorValue(GrColor_t value) {
    Logger::debug(">>> grConstantColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetConstantColor(value);
  }

  DLLEXPORT void __stdcall grCullMode(GrCullMode_t mode) {
    Logger::debug(">>> grCullMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCullMode(mode);
  }

  DLLEXPORT void __stdcall grDepthBiasLevel(FxI16 level) {
    Logger::debug(">>> grDepthBiasLevel");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBiasLevel(level);
  }

  DLLEXPORT void __stdcall grDepthBufferFunction(GrCmpFnc_t function) {
    Logger::debug(">>> grDepthBufferFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBufferFunction(function);
  }

  DLLEXPORT void __stdcall grDepthBufferMode(GrDepthBufferMode_t mode) {
    Logger::debug(">>> grDepthBufferMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthBufferMode(mode);
  }

  DLLEXPORT void __stdcall grDepthMask(FxU32 mask) {
    Logger::debug(">>> grDepthMask");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthMask(mask);
  }

  DLLEXPORT void __stdcall grDisableAllEffects(void) {
    Logger::debug("stub: grDisableAllEffects");
  }

  DLLEXPORT void __stdcall grDitherMode(GrDitherMode_t mode) {
    Logger::debug(">>> grDitherMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDitherMode(mode);
  }

  DLLEXPORT void __stdcall grDrawLine(const glide2x::GrVertex *v1, const glide2x::GrVertex *v2) {
    Logger::debug(">>> grDrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grDrawPlanarPolygon(int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grDrawPlanarPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grDrawPlanarPolygonVertexList(int nverts, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grDrawPlanarPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grDrawPoint(const glide2x::GrVertex *pt) {
    Logger::debug(">>> grDrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grDrawPolygon(int nverts, const int *ilist, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grDrawPolygon");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayIndexed(glide3x::GR_POLYGON, nverts, vlist, ilist);
  }

  DLLEXPORT void __stdcall grDrawPolygonVertexList(int nverts, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> grDrawPolygonVertexList");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall grDrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    Logger::debug(">>> grDrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grErrorSetCallback(GrErrorCallbackFnc_t fnc) {
    Logger::debug("stub: grErrorSetCallback");
  }

  DLLEXPORT void __stdcall grFogColorValue(GrColor_t fogcolor) {
    Logger::debug("stub: grFogColorValue");
  }

  DLLEXPORT void __stdcall grFogMode(glide2x::GrFogMode_t mode) {
    Logger::debug("stub: grFogMode");
  }

  DLLEXPORT void __stdcall grFogTable(const GrFog_t *ft) {
    Logger::debug("stub: grFogTable");
  }

  DLLEXPORT void __stdcall grGammaCorrectionValue(float value) {
    Logger::debug(">>> grGammaCorrectionValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetGammaCorrection(value, value, value);
  }

  DLLEXPORT void __stdcall grGlideGetState(glide2x::GrState *state) {
    Logger::debug(">>> grGlideGetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->LoadState(state);
  }

  DLLEXPORT void __stdcall grGlideGetVersion(char version[80]) {
    Logger::debug(">>> grGlideGetVersion");
    if (version == nullptr)
      return;

    std::strncpy(version, GlideVersion, 79);
  }

  DLLEXPORT void __stdcall grGlideInit(void) {
    Logger::debug(">>> grGlideInit");

    initDXVKDevice();

    glideDevice->SetCoordinateSpace(glide3x::GR_WINDOW_COORDS);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_XY, glide3x::GR_PARAM_ENABLE, 0);
    // 3Dfx headers says that Z is ignored, so not likely to be populated by games
    // glideDevice->SetVertexLayout(glide3x::GR_PARAM_Z, glide3x::GR_PARAM_ENABLE, 8);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_RGB, glide3x::GR_PARAM_ENABLE, 12);
    // ooz
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Z, glide3x::GR_PARAM_ENABLE, 24);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_A, glide3x::GR_PARAM_ENABLE, 28);
    // oow, 3Dfx porting guide advice to use GR_PARAM_W but we temporarily use GR_PARAM_Q
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Q, glide3x::GR_PARAM_ENABLE, 32);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_ST0, glide3x::GR_PARAM_ENABLE, 36);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Q0, glide3x::GR_PARAM_ENABLE, 44);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_ST1, glide3x::GR_PARAM_ENABLE, 48);
    glideDevice->SetVertexLayout(glide3x::GR_PARAM_Q1, glide3x::GR_PARAM_ENABLE, 56);
  }

  DLLEXPORT void __stdcall grGlideSetState(const glide2x::GrState *state) {
    Logger::debug(">>> grGlideSetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SaveState(state);
  }

  DLLEXPORT void __stdcall grGlideShamelessPlug(const FxBool on) {
    Logger::debug("stub: grGlideShamelessPlug");
  }

  DLLEXPORT void __stdcall grGlideShutdown(void) {
    Logger::debug(">>> grGlideShutdown");

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
    Logger::debug(">>> grHints");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetHints(hintType, hintMask);
  }

  DLLEXPORT void __stdcall grLfbConstantAlpha(GrAlpha_t alpha) {
    Logger::debug("stub: grLfbConstantAlpha");
  }

  DLLEXPORT void __stdcall grLfbConstantDepth(FxU16 depth) {
    Logger::debug("stub: grLfbConstantDepth");
  }

  DLLEXPORT FxBool __stdcall grLfbLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info) {
    Logger::debug(">>> grLfbLock");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    return glideDevice->LinearBufferLock(type, buffer, writeMode, origin, pixelPipeline, info);
  }

  DLLEXPORT FxBool __stdcall grLfbReadRegion(GrBuffer_t buffer, FxU32 x, FxU32 y, FxU32 width, FxU32 height, FxU32 stride, void *data) {
    Logger::debug("stub: grLfbReadRegion");
    if (unlikely(data == nullptr))
      return FXFALSE;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, buffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    // TODO

    if (!glideDevice->LinearBufferUnlock(GR_LFB_READ_ONLY, buffer))
      return FXFALSE;

    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall grLfbUnlock(GrLock_t type, GrBuffer_t buffer) {
    Logger::debug(">>> grLfbUnlock");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    return glideDevice->LinearBufferUnlock(type, buffer);
  }

  DLLEXPORT void __stdcall grLfbWriteColorFormat(FxI32 colorFormat) {
    Logger::debug("stub: grLfbWriteColorFormat");
  }

  DLLEXPORT void __stdcall grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords) {
    Logger::debug("stub: grLfbWriteColorSwizzle");
  }

  DLLEXPORT FxBool __stdcall grLfbWriteRegion(GrBuffer_t buffer, FxU32 x, FxU32 y, FxU32 format, FxU32 width, FxU32 height, FxI32 stride, void *data) {
    Logger::debug("stub: grLfbWriteRegion");
    return FXTRUE;

    if (unlikely(data == nullptr))
      return FXFALSE;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, buffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    // TODO

    if (!glideDevice->LinearBufferUnlock(GR_LFB_READ_ONLY, buffer))
      return FXFALSE;

    return FXTRUE;
  }

  // Not in 3Dfx but nGlide has this for extended functionality? Most likely no real users but we'll add it too.
  DLLEXPORT FxI32 __stdcall grQueryResolutions(const glide3x::GrResolution *resTemplate, glide3x::GrResolution *output) {
    Logger::debug(">>> grQueryResolutions");
    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->QueryResolutions(resTemplate, output);
  }

  DLLEXPORT void __stdcall grRenderBuffer(GrBuffer_t buffer) {
    Logger::debug(">>> grRenderBuffer");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetRenderBuffer(buffer);
  }

  DLLEXPORT void __stdcall grResetTriStats(void) {
    Logger::debug("stub: grResetTriStats");
  }

  DLLEXPORT void __stdcall grSplash(float x, float y, float width, float height, FxU32 frame) {
    Logger::debug("stub: grSplash");
  }

  DLLEXPORT void __stdcall grSstConfigPipeline(GrChipID_t chip, GrSstRegister reg, FxU32 value) {
    Logger::debug("stub: grSstConfigPipeline");
  }

  DLLEXPORT FxBool __stdcall grSstControl(glide2x::GrControl_t code) {
    Logger::debug("stub: grSstControl");
    return FXTRUE;
  }

  DLLEXPORT void __stdcall grSstIdle(void) {
    Logger::debug("stub: grSstIdle");
  }

  DLLEXPORT FxBool __stdcall grSstIsBusy(void) {
    Logger::debug("stub: grSstIsBusy");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall grSstOrigin(FxI32 origin) {
    Logger::debug("stub: grSstOrigin");
  }

  DLLEXPORT void __stdcall grSstPerfStats(GrSstPerfStats_t *pStats) {
    Logger::debug("stub: grSstPerfStats");
  }

  DLLEXPORT FxBool __stdcall grSstQueryBoards(GrHwConfiguration *hwconfig) {
    Logger::debug(">>> grSstQueryBoards");

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
    Logger::debug(">>> grSstQueryHardware");

    if (unlikely(hwconfig == nullptr))
      return FXFALSE;

    // Could be called before grGlideInit
    initDXVKDevice();

    const GrHwConfiguration *hwConfig = glideDevice->GetHWConfiguration();
    memcpy(hwconfig, hwConfig, sizeof(GrHwConfiguration));

    return FXTRUE;
  }

  DLLEXPORT void __stdcall grSstResetPerfStats(void) {
    Logger::debug("stub: grSstResetPerfStats");
  }

  DLLEXPORT FxU32 __stdcall grSstScreenHeight(void) {
    Logger::debug(">>> grSstScreenHeight");

    if (unlikely(glideDevice == nullptr))
      return 0;

    FxU32 height;
    glideDevice->GetBackBufferDimensions(nullptr, &height);
    Logger::debug(str::format(">>> grSstScreenHeight: ", height));

    return height;
  }

  DLLEXPORT FxU32 __stdcall grSstScreenWidth(void) {
    Logger::debug(">>> grSstScreenWidth");

    if (unlikely(glideDevice == nullptr))
      return 0;

    FxU32 width;
    glideDevice->GetBackBufferDimensions(&width, nullptr);

    return width;
  }

  DLLEXPORT void __stdcall grSstSelect(int sst) {
    Logger::debug(">>> grSstSelect");

    // We are emulating a single card
    if (unlikely(sst != 0)) {
      Logger::warn(str::format("Unexpected hwcard requested: ", sst));
    }
  }

  DLLEXPORT FxU32 __stdcall grSstStatus(void) {
    Logger::debug("stub: grSstStatus");
    return 0;
  }

  DLLEXPORT FxBool __stdcall grSstVRetraceOn(void) {
    Logger::debug("stub: grSstVRetraceOn");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall grSstVidMode(FxU32 whichSst, FxVideoTimingInfo *vidTimings) {
    Logger::debug("stub: grSstVidMode");
  }

  DLLEXPORT FxU32 __stdcall grSstVideoLine(void) {
    Logger::debug("stub: grSstVideoLine");
    return 0;
  }

  DLLEXPORT void __stdcall grSstWinClose(void) {
    Logger::debug(">>> grSstWinClose");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->WindowClose();
  }

  DLLEXPORT FxBool __stdcall grSstWinOpen(FxU32 hWnd,
                                          GrScreenResolution_t resolution,
                                          GrScreenRefresh_t refreshRate,
                                          GrColorFormat_t colorFormat,
                                          GrOriginLocation_t originLocation,
                                          int nColBuffers, int nAuxBuffers) {
    Logger::debug(">>> grSstWinOpen");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    HWND m_hWnd = reinterpret_cast<HWND>(hWnd);
    if (m_hWnd == nullptr)
      m_hWnd = GetActiveWindow();

    return glideDevice->WindowOpen(m_hWnd, resolution, refreshRate, colorFormat, originLocation, nColBuffers, nAuxBuffers);
  }

  DLLEXPORT FxU32 __stdcall grTexCalcMemRequired(glide2x::GrLOD_t lodmin, glide2x::GrLOD_t lodmax, glide2x::GrAspectRatio_t aspect, GrTextureFormat_t fmt) {
    Logger::debug(">>> grTexCalcMemRequired");

    return UtilGetTextureSize(UtilLodToLOG2(lodmin), UtilLodToLOG2(lodmax), UtilAspectRatioToLOG2(aspect), fmt);
  }

  DLLEXPORT void __stdcall grTexClampMode(GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode) {
    Logger::debug(">>> grTexClampMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexClampMode(tmu, s_clampmode, t_clampmode);
  }

  DLLEXPORT void __stdcall grTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert) {
    Logger::debug(">>> grTexCombine");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombine(tmu, rgb_function, rgb_factor, alpha_function, alpha_factor, rgb_invert, alpha_invert);
  }

  DLLEXPORT void __stdcall grTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc) {
    Logger::debug(">>> grTexCombineFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombineFunction(tmu, fnc);
  }

  DLLEXPORT void __stdcall grTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, float detail_max) {
    Logger::debug(">>> grTexDetailControl");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexDetailControl(tmu, lod_bias, detail_scale, detail_max);
  }

  DLLEXPORT void __stdcall grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    Logger::debug(">>> grTexDownloadMipMap");

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

  DLLEXPORT void __stdcall grTexDownloadMipMapLevel(GrChipID_t tmu, FxU32 startAddress, glide2x::GrLOD_t thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data) {
    Logger::debug(">>> grTexDownloadMipMapLevel");

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

  DLLEXPORT void __stdcall grTexDownloadMipMapLevelPartial(GrChipID_t tmu, FxU32 startAddress, glide2x::GrLOD_t thisLod, glide2x::GrLOD_t largeLod, glide2x::GrAspectRatio_t  aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data, FxI32 start, FxI32 end) {
    Logger::debug(">>> grTexDownloadMipMapLevelPartial");

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
    Logger::debug(">>> grTexDownloadTable");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    TMU->SetTable(type, data, 0, 0);
  }

  DLLEXPORT void __stdcall grTexDownloadTablePartial(GrChipID_t tmu, GrTexTable_t type, void *data, int start, int end) {
    Logger::debug(">>> grTexDownloadTablePartial");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    TMU->SetTable(type, data, start, end);
  }

  DLLEXPORT void __stdcall grTexFilterMode(GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode) {
    Logger::debug(">>> grTexFilterMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexFilterMode(tmu, minfilter_mode, magfilter_mode);
  }

  DLLEXPORT void __stdcall grTexLodBiasValue(GrChipID_t tmu, FxFloat bias) {
    Logger::debug(">>> grTexLodBiasValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexLodBiasValue(tmu, bias);
  }

  DLLEXPORT FxU32 __stdcall grTexMaxAddress(GrChipID_t tmu) {
    Logger::debug(">>> grTexMaxAddress");

    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->GetTMUMaxAddress(tmu);
  }

  DLLEXPORT FxU32 __stdcall grTexMinAddress(GrChipID_t tmu) {
    Logger::debug(">>> grTexMinAddress");

    if (unlikely(glideDevice == nullptr))
      return 0;

    return glideDevice->GetTMUMinAddress(tmu);
  }

  DLLEXPORT void __stdcall grTexMipMapMode(GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend) {
    Logger::debug(">>> grTexMipMapMode");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetTexMipMapMode(tmu, mode, lodBlend);
  }

  DLLEXPORT void __stdcall grTexMultibase(GrChipID_t tmu, FxBool enable) {
    Logger::debug("stub: grTexMultibase");
  }

  DLLEXPORT void __stdcall grTexMultibaseAddress(GrChipID_t tmu, FxU32 range, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    Logger::debug("stub: grTexMultibaseAddress");
  }

  DLLEXPORT void __stdcall grTexNCCTable(GrChipID_t tmu, GrTexTable_t table) {
    Logger::debug(">>> grTexNCCTable");

    if (table != GR_TEXTABLE_NCC0 && table != GR_TEXTABLE_NCC1)
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    TMU->SelectNCCTable(table);
  }

  DLLEXPORT void __stdcall grTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    Logger::debug(">>> grTexSource");

    if (info == nullptr)
      return;

    glideDevice->SetTexSource(tmu, startAddress, evenOdd, UtilLodToLOG2(info->smallLod), UtilLodToLOG2(info->largeLod), UtilAspectRatioToLOG2(info->aspectRatio), info->format);
  }

  DLLEXPORT FxU32 __stdcall grTexTextureMemRequired(MipMapLevelMask_t evenOdd, glide2x::GrTexInfo *info) {
    Logger::debug(">>> grTexTextureMemRequired");

    if (info == nullptr)
      return 0;

    return UtilGetTextureSize(UtilLodToLOG2(info->smallLod), UtilLodToLOG2(info->largeLod), UtilAspectRatioToLOG2(info->aspectRatio), info->format, evenOdd);
  }

  DLLEXPORT void __stdcall grTriStats(FxU32 *trisProcessed, FxU32 *trisDrawn) {
    Logger::debug("stub: grTriStats");
  }

  DLLEXPORT FxBool __stdcall gu3dfGetInfo(const char *filename, glide2x::Gu3dfInfo *info) {
    Logger::debug("stub: gu3dfGetInfo");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall gu3dfLoad(const char *filename, glide2x::Gu3dfInfo *data) {
    Logger::debug("stub: gu3dfLoad");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guAADrawTriangleWithClip(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    Logger::debug(">>> guAADrawTriangleWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall guAlphaSource(GrAlphaSource_t mode) {
    Logger::debug(">>> guAlphaSource");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetAlphaSource(mode);
  }

  DLLEXPORT void __stdcall guColorCombineFunction(GrColorCombineFnc_t fnc) {
    Logger::debug(">>> guColorCombineFunction");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetColorCombineFunction(fnc);
  }

  DLLEXPORT void __stdcall guDrawPolygonVertexListWithClip(int nverts, const glide2x::GrVertex *vlist) {
    Logger::debug(">>> guDrawPolygonVertexListWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(glide3x::GR_POLYGON, nverts, vlist);
  }

  DLLEXPORT void __stdcall guDrawTriangleWithClip(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    Logger::debug(">>> guDrawTriangleWithClip");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT int __stdcall guEncodeRLE16(void *dst, void *src, FxU32 width, FxU32 height) {
    Logger::debug("stub: guEncodeRLE16");
    return 0;
  }

  DLLEXPORT FxU16 __stdcall guEndianSwapBytes(FxU16 value) {
    Logger::debug(">>> guEndianSwapBytes");

    return (value << 8) | (value >> 8);
  }

  DLLEXPORT FxU32 __stdcall guEndianSwapWords(FxU32 value) {
    Logger::debug(">>> guEndianSwapWords");

    return (value << 16) | (value >> 16);
  }

  DLLEXPORT void __stdcall guFbReadRegion(const int srcX, const int srcY, const int w, const int h, const void *dst, const int strideInBytes) {
    Logger::debug("stub: guFbReadRegion");
  }

  DLLEXPORT void __stdcall guFbWriteRegion(const int dstX, const int dstY, const int w, const int h, const void *src, const int strideInBytes) {
    Logger::debug("stub: guFbWriteRegion");
  }

  DLLEXPORT void __stdcall guFogGenerateExp(GrFog_t *fogTable, float density) {
    Logger::debug(">>> guFogGenerateExp");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateExp2(GrFog_t *fogTable, float density) {
    Logger::debug(">>> guFogGenerateExp2");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp2(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateLinear(GrFog_t *fogTable, float nearZ, float farZ) {
    Logger::debug(">>> guFogGenerateLinear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateLinear(fogTable, nearZ, farZ);
  }

  DLLEXPORT FxFloat __stdcall guFogTableIndexToW(FxI32 i) {
    Logger::debug(">>> guFogTableIndexToW");

    if (unlikely(glideDevice == nullptr))
      return 0.0;

    return glideDevice->UtilFogTableIndexToW(i);
  }

  DLLEXPORT void __stdcall guMPDrawTriangle(const glide2x::GrVertex *a, const glide2x::GrVertex *b, const glide2x::GrVertex *c) {
    Logger::debug("stub: guMPDrawTriangle");
  }

  DLLEXPORT void __stdcall guMPInit(void) {
    Logger::debug("stub: guMPInit");
  }

  DLLEXPORT void __stdcall guMPTexCombineFunction(GrMPTextureCombineFnc_t tc) {
    Logger::debug("stub: guMPTexCombineFunction");
  }

  DLLEXPORT void __stdcall guMPTexSource(GrChipID_t virtual_tmu, GrMipMapId_t mmid) {
    Logger::debug("stub: guMPTexSource");
  }

  DLLEXPORT void __stdcall guMovieSetName(const char *name) {
    Logger::debug("stub: guMovieSetName");
  }

  DLLEXPORT void __stdcall guMovieStart(void) {
    Logger::debug("stub: guMovieStart");
  }

  DLLEXPORT void __stdcall guMovieStop(void) {
    Logger::debug("stub: guMovieStop");
  }

  DLLEXPORT GrMipMapId_t __stdcall guTexAllocateMemory(GrChipID_t tmu, FxU8 odd_even_mask, FxI32 width, FxI32 height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, glide2x::GrLOD_t smallest_lod, glide2x::GrLOD_t largest_lod, glide2x::GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode, FxFloat lod_bias, FxBool trilinear) {
    Logger::debug("stub: guTexAllocateMemory");
    return 0;
  }

  DLLEXPORT FxBool __stdcall guTexChangeAttributes(GrMipMapId_t mmid, FxI32 width, FxI32 height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, glide2x::GrLOD_t smallest_lod, glide2x::GrLOD_t largest_lod, glide2x::GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minFilterMode, GrTextureFilterMode_t magFilterMode) {
    Logger::debug("stub: guTexChangeAttributes");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guTexCombineFunction(GrChipID_t tmu, GrTextureCombineFnc_t fnc) {
    Logger::debug(">>> guTexCombineFunction");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexCombineFunction(tmu, fnc);
  }

  DLLEXPORT FxU16 *__stdcall guTexCreateColorMipMap(void) {
    Logger::debug("stub: guTexCreateColorMipMap");
    return 0;
  }

  DLLEXPORT void __stdcall guTexDownloadMipMap(GrMipMapId_t mmid, const void *src, const GuNccTable *table) {
    Logger::debug("stub: guTexDownloadMipMap");
  }

  DLLEXPORT void __stdcall guTexDownloadMipMapLevel(GrMipMapId_t mmid, FxI32 lod, const void ** src) {
    Logger::debug("stub: guTexDownloadMipMapLevel");
  }

  DLLEXPORT GrMipMapId_t __stdcall guTexGetCurrentMipMap(GrChipID_t tmu) {
    Logger::debug("stub: guTexGetCurrentMipMap");
    return 0;
  }

  DLLEXPORT glide2x::GrMipMapInfo* __stdcall guTexGetMipMapInfo(GrMipMapId_t mmid) {
    Logger::debug("stub: guTexGetMipMapInfo");
    return nullptr;
  }

  DLLEXPORT FxU32 __stdcall guTexMemQueryAvail(GrChipID_t tmu) {
    Logger::debug("stub: guTexMemQueryAvail");
    return 0;
  }

  DLLEXPORT void __stdcall guTexMemReset(void) {
    Logger::debug("stub: guTexMemReset");
  }

  DLLEXPORT void __stdcall guTexSource(GrMipMapId_t id) {
    Logger::debug("stub: guTexSource");
  }

  DLLEXPORT FxBool __stdcall pciClose(void) {
    Logger::debug("stub: pciClose");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciDeviceExists(FxU32 device_number) {
    Logger::debug("stub: pciDeviceExists");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindCardMulti(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum, FxU32 cardNum) {
    Logger::debug("stub: pciFindCardMulti");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindCard(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum) {
    return pciFindCardMulti(vendorID, deviceID, devNum, 0);
  }

  DLLEXPORT FxBool __stdcall pciFindCardMultiFunc(FxU32 vendorID, FxU32 deviceID, FxU32 *devNum, FxU32 *funcNum, FxU32 functionIndex) {
    Logger::debug("stub: pciFindCardMultiFunc");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindMTRRMatch(FxU32 pBaseAddrs, FxU32 psz, PciMemType type, FxU32 *mtrrNum) {
    Logger::debug("stub: pciFindMTRRMatch");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciFindFreeMTRR(FxU32 *mtrrNum) {
    Logger::debug("stub: pciFindFreeMTRR");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciGetConfigData(PciRegister reg, FxU32 device_number, FxU32 *data) {
    Logger::debug("stub: pciGetConfigData");
    return FXFALSE;
  }

  DLLEXPORT FxU32 __stdcall pciGetErrorCode(void) {
    Logger::debug("stub: pciGetErrorCode");
    return 0;
  }

  DLLEXPORT const char* __stdcall pciGetErrorString(void) {
    Logger::debug("stub: pciGetErrorString");
    return nullptr;
  }

  DLLEXPORT FxU32 * __stdcall pciMapCard(FxU32 vID, FxU32 dID, FxI32 len, FxU32 *devNo, FxU32 addrNo) {
    Logger::debug("stub: pciMapCard");
    return 0;
  }

  DLLEXPORT FxU32 * __stdcall pciMapCardMulti(FxU32 vID,FxU32 dID,FxI32 l,FxU32 *dNo,FxU32 cNo,FxU32 aNo) {
    Logger::debug("stub: pciMapCardMulti");
    return 0;
  }

  DLLEXPORT FxBool __stdcall pciMapPhysicalToLinear(unsigned long *linear_addr, FxU32 physical_addr,FxU32 *length) {
    Logger::debug("stub: pciMapPhysicalToLinear");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciOpen(void) {
    Logger::debug("stub: pciOpen");
    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall pciOutputDebugString(const char* debugMsg) {
    Logger::debug("stub: pciOutputDebugString");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetConfigData(PciRegister reg, FxU32 device_number, FxU32 *data) {
    Logger::debug("stub: pciSetConfigData");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetMTRR(FxU32 mtrrNo, FxU32 pBaseAddr, FxU32 psz, PciMemType type) {
    Logger::debug("stub: pciSetMTRR");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall pciSetPassThroughBase(FxU32* pBaseAddr, FxU32 baseAddrLen) {
    Logger::debug("stub: pciSetPassThroughBase");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall pciUnmapPhysical(unsigned long linear_addr, FxU32 length) {
    Logger::debug("stub: pciUnmapPhysical");
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
