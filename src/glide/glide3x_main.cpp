#include "glide_include.h"

#include "glide_device.h"
#include "glide_options.h"
#include "glide_utils.h"

#include "../util/util_singleton.h"

namespace dxvk {
  Logger Logger::s_instance("glide3x.log");
  static GlideDevice* glideDevice = nullptr;
  constexpr FxI32 OneMB = 1024 * 1024;

  static inline void initDXVKDevice () {
    if (glideDevice != nullptr)
      return;

    try {
      glideDevice = new GlideDevice(GLIDEAPI::API_GLIDE_3X);
    } catch (const DxvkError& e) {
      Logger::err(e.message());
      exit(0);
    }
  }
}

extern "C" {
  using namespace dxvk;

  // Extensions
  void grChromaRangeExt(GrColor_t color0, GrColor_t color1, GrChromaRangeMode_t mode) {
    DEBUGLOG("stub: grChromaRangeExt");
  }

  void grChromaRangeModeExt(GrChromaRangeMode_t mode) {
    DEBUGLOG("stub: grChromaRangeModeExt");
  }

  void grTexChromaModeExt(GrChipID_t tmu, GrChromakeyMode_t mode) {
    DEBUGLOG("stub: grTexChromaModeExt");
  }

  void grTexChromaRangeExt(GrChipID_t tmu, GrColor_t color0, GrColor_t color1, GrTexChromakeyMode_t mode)   {
    DEBUGLOG("stub: grTexChromaRangeExt");
  }

  // API
  DLLEXPORT void __stdcall grAADrawTriangle(const void *a, const void *b, const void *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias) {
    DEBUGLOG("stub: grAADrawTriangle");
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

  DLLEXPORT void __stdcall grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth) {
    DEBUGLOG(">>> grBufferClear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferClear(color, alpha, depth);
  }

  DLLEXPORT void __stdcall grBufferSwap(FxU32 swap_interval) {
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

  DLLEXPORT void __stdcall grClipWindow(FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy) {
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

  DLLEXPORT void __stdcall grConstantColorValue(GrColor_t value) {
    DEBUGLOG(">>> grConstantColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetConstantColor(value);
  }

  DLLEXPORT void __stdcall grCoordinateSpace(glide3x::GrCoordinateSpaceMode_t mode) {
    DEBUGLOG(">>> grCoordinateSpace");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCoordinateSpace(mode);
  }

  DLLEXPORT void __stdcall grCullMode(GrCullMode_t mode) {
    DEBUGLOG(">>> grCullMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCullMode(mode);
  }

  DLLEXPORT void __stdcall grDepthBiasLevel(FxI32 level) {
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

  DLLEXPORT void __stdcall grDepthRange(FxFloat n, FxFloat f) {
    DEBUGLOG(">>> grDepthRange");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthRange(n, f);
  }

  DLLEXPORT void __stdcall grDisable(GrEnableMode_t mode) {
    DEBUGLOG("stub: grDisable");
  }

  DLLEXPORT void __stdcall grDisableAllEffects() {
    DEBUGLOG("stub: grDisableAllEffects");
  }

  DLLEXPORT void __stdcall grDitherMode(GrDitherMode_t mode) {
    DEBUGLOG(">>> grDitherMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDitherMode(mode);
  }

  DLLEXPORT void __stdcall grDrawLine(const void *v1, const void *v2) {
    DEBUGLOG(">>> grDrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grDrawPoint(const void *pt) {
    DEBUGLOG(">>> grDrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grDrawTriangle(const void *a, const void *b, const void *c) {
    DEBUGLOG(">>> grDrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grDrawVertexArray(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, void *pointers) {
    DEBUGLOG(">>> grDrawVertexArray");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArray(mode, count, pointers);
  }

  DLLEXPORT void __stdcall grDrawVertexArrayContiguous(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, void *pointers, FxU32 stride) {
    DEBUGLOG(">>> grDrawVertexArrayContiguous");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(mode, count, pointers, stride);
  }

  DLLEXPORT void __stdcall grEnable(GrEnableMode_t mode) {
    DEBUGLOG("stub: grEnable");
  }

  DLLEXPORT void __stdcall grErrorSetCallback(GrErrorCallbackFnc_t fnc) {
    DEBUGLOG("stub: grErrorSetCallback");
  }

  DLLEXPORT void __stdcall grFinish() {
    DEBUGLOG(">>> grFinish");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->Flush();
  }

  DLLEXPORT void __stdcall grFlush() {
    DEBUGLOG(">>> grFlush");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->ConsiderFlush(GpuFlushType::ImplicitWeakHint);
  }

  DLLEXPORT void __stdcall grFogColorValue(GrColor_t fogColor) {
    DEBUGLOG(">>> grFogColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetFogColor(fogColor);
  }

  DLLEXPORT void __stdcall grFogMode(glide3x::GrFogMode_t mode) {
    DEBUGLOG(">>> grFogMode");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->SetFogMode(mode);
  }

  DLLEXPORT void __stdcall grFogTable(const GrFog_t *fogTable) {
    DEBUGLOG(">>> grFogTable");

    if (unlikely(glideDevice == nullptr || fogTable == nullptr))
      return;

    return glideDevice->SetFogTable(fogTable);
  }

  DLLEXPORT FxU32 __stdcall grGet(glide3x::GrGetReset_t pname, FxU32 plength, FxI32 *params) {
    DEBUGLOG(str::format(">>> grGet: ", pname));

    switch (pname) {
      case glide3x::GR_BITS_DEPTH:
        if (likely(plength >= 4))
          params[0] = 16;
        break;
      case glide3x::GR_BITS_GAMMA:
        if (likely(plength >= 4))
          params[0] = 8;
        break;
      case glide3x::GR_BITS_RGBA:
        if (likely(plength >= 4))
          params[0] = 32;
        break;
      case glide3x::GR_FOG_TABLE_ENTRIES:
        if (likely(plength >= 4))
          params[0] = FOG_TABLE_ENTRIES_COUNT;
        break;
      case glide3x::GR_GAMMA_TABLE_ENTRIES:
        if (likely(plength >= 4))
          params[0] = GAMMA_TABLE_ENTRIES_COUNT;
        break;
      case glide3x::GR_GLIDE_STATE_SIZE:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr))
            params[0] = glideDevice->GlideStateSize();
          else
            params[0] = 0;
        }
        break;
      case glide3x::GR_GLIDE_VERTEXLAYOUT_SIZE:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr))
            params[0] = glideDevice->GetVertexLayoutSize();
          else
            params[0] = 0;
        }
        break;
      // TODO: look if there are any users which care about that
      case glide3x::GR_LFB_PIXEL_PIPE:
        if (likely(plength >= 4))
          params[0] = 0;
        break;
      case glide3x::GR_MAX_TEXTURE_ASPECT_RATIO:
        if (likely(plength >= 4))
          params[0] = glide3x::GR_ASPECT_LOG2_8x1;
        break;
      case glide3x::GR_MAX_TEXTURE_SIZE:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->maxTextureSize;
          } else {
            params[0] = 0;
          }
        }
        break;
      case glide3x::GR_MEMORY_FB:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            if (boardConfig->uma)
              params[0] = 0;
            else
              params[0] = boardConfig->fbRam * OneMB;
          }
        }
        break;
      case glide3x::GR_MEMORY_TMU:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr))
            params[0] = glideDevice->GetTMUMemory() * OneMB;
          else
            params[0] = 0;
        }
        break;
      case glide3x::GR_MEMORY_UMA:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            if (boardConfig->uma)
              params[0] = glideDevice->GetTMUMemory() * OneMB * boardConfig->tmuCount;
            else
            params[0] = 0;
          } else {
            params[0] = 0;
          }
        }
        break;
      case glide3x::GR_NON_POWER_OF_TWO_TEXTURES:
        // Docs state "Not implemented in the initial release of Glide 3.0." so maybe was never supported?
        if (likely(plength >= 4))
          params[0] = 0;
        break;
      case glide3x::GR_NUM_BOARDS:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr))
            params[0] = glideDevice->GetHWConfiguration()->num_sst;
          else
            params[0] = 1;
        }
        break;
      case glide3x::GR_NUM_FB:
        if (likely(plength >= 4))
          params[0] = 1;
        break;
      case glide3x::GR_NUM_TMU:
        if (likely(plength >= 4)) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GetTMUCount();
          else
            params[0] = 0;
        }
        break;
      case glide3x::GR_REVISION_FB:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->fbRev;
          } else {
            params[0] = 0;
          }
        }
        break;
      case glide3x::GR_REVISION_TMU:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->tmuRev;
          } else {
            params[0] = 0;
          }
        }
        break;
      case glide3x::GR_SUPPORTS_PASSTHRU:
        if (likely(plength >= 4)) {
          if (likely(glideDevice != nullptr)) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->passthrough;
          } else {
            params[0] = 0;
          }
        }
        break;
      case glide3x::GR_TEXTURE_ALIGN:
        if (likely(plength >= 4))
          params[0] = SST1_TEXTURE_ALIGN;
        break;
      case glide3x::GR_VIEWPORT:
        if (likely(plength >= 16)) {
          if (likely(glideDevice != nullptr)) {
            FxU32 x, y, width, height;
            glideDevice->GetViewportDimensions(&x, &y, &width, &height);
            params[0] = x;
            params[1] = y;
            params[2] = width;
            params[3] = height;
          } else {
            params[0] = 0;
            params[1] = 0;
            params[2] = 0;
            params[3] = 0;
          }
        }
        break;
      case glide3x::GR_WDEPTH_MIN_MAX:
        if (likely(plength >= 8)) {
          params[0] = SST1_WDEPTHVALUE_NEAREST;
          params[1] = SST1_WDEPTHVALUE_FARTHEST;
        }
        break;
      case glide3x::GR_ZDEPTH_MIN_MAX:
        if (likely(plength >= 8)) {
          params[0] = SST1_ZDEPTHVALUE_NEAREST;
          params[1] = SST1_ZDEPTHVALUE_FARTHEST;
        }
        break;
      // we are not in CRT days, beam is always 0,0
      case glide3x::GR_VIDEO_POSITION:
        if (likely(plength >= 8)) {
          params[0] = 0;
          params[1] = 0;
        }
        break;
      // we are never busy
      case glide3x::GR_FIFO_FULLNESS:
      case glide3x::GR_IS_BUSY:
      case glide3x::GR_PENDING_BUFFERSWAPS:
      // we are not reporting stats back
      case glide3x::GR_STATS_LINES:
      case glide3x::GR_STATS_PIXELS_AFUNC_FAIL:
      case glide3x::GR_STATS_PIXELS_CHROMA_FAIL:
      case glide3x::GR_STATS_PIXELS_DEPTHFUNC_FAIL:
      case glide3x::GR_STATS_PIXELS_IN:
      case glide3x::GR_STATS_PIXELS_OUT:
      case glide3x::GR_STATS_PIXELS:
      case glide3x::GR_STATS_POINTS:
      case glide3x::GR_STATS_TRIANGLES_IN:
      case glide3x::GR_STATS_TRIANGLES_OUT:
      case glide3x::GR_STATS_TRIANGLES:
      case glide3x::GR_SWAP_HISTORY:
        if (likely(plength >= 4)) {
          params[0] = 0;
        }
        break;
      default:
        Logger::warn(str::format("Unknown grGet pname: ", pname));
        if (likely(plength >= 4)) {
          params[0] = 0;
        }
        break;
    }

    return plength;
  }

  DLLEXPORT const char* __stdcall grGetString(glide3x::GrGetParam_t pname) {
    DEBUGLOG(">>> grGetString");

    switch (pname) {
      case glide3x::GR_EXTENSION:
        // TODO: PALETTE6666 is buggy for some reason in NFS5, investigate
        return "CHROMARANGE FOGCOORD TEXCHROMA TEXMIRROR ";
      case glide3x::GR_HARDWARE:
        if (likely(glideDevice != nullptr))
          return glideDevice->GetBoardConfiguration()->name.c_str();
        return "GXVK";
      case glide3x::GR_RENDERER:
        return "Glide";
      case glide3x::GR_VENDOR:
        return "GXVK";
      case glide3x::GR_VERSION:
        return "3.0";
    }

    return nullptr;
  }

  DLLEXPORT void __stdcall grGlideGetState(void *state) {
    DEBUGLOG(">>> grGlideGetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->GetState(state);
  }

  DLLEXPORT void __stdcall grGlideGetVertexLayout(void *layout) {
    DEBUGLOG(">>> grGlideGetVertexLayout");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->LoadVertexLayout(layout);
  }

  DLLEXPORT void __stdcall grGlideInit() {
    DEBUGLOG(">>> grGlideInit");

    initDXVKDevice();
  }

  DLLEXPORT void __stdcall grGlideSetState(const void *state) {
    DEBUGLOG(">>> grGlideSetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetState(state);
  }

  DLLEXPORT void __stdcall grGlideSetVertexLayout(const void *layout) {
    DEBUGLOG(">>> grGlideSetVertexLayout");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SaveVertexLayout(layout);
  }

  DLLEXPORT void __stdcall grGlideShutdown() {
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

  DLLEXPORT void __stdcall grLfbConstantAlpha(GrAlpha_t alpha) {
    DEBUGLOG("stub: grLfbConstantAlpha");
  }

  DLLEXPORT void __stdcall grLfbConstantDepth(FxU32 depth) {
    DEBUGLOG("stub: grLfbConstantDepth");
  }

  DLLEXPORT FxBool __stdcall grLfbLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info) {
    DEBUGLOG(">>> grLfbLock");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    return glideDevice->LinearBufferLock(type, buffer, writeMode, origin, pixelPipeline, info);
  }

  DLLEXPORT FxBool __stdcall grLfbReadRegion(GrBuffer_t srcBuffer, FxU32 srcX, FxU32 srcY, FxU32 srcWidth, FxU32 srcHeight, FxU32 dstStride, void *dstData) {
    DEBUGLOG(">>> grLfbReadRegion");

    if (unlikely(glideDevice == nullptr || dstData == nullptr))
      return FXFALSE;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    FxU32 blockSize = 2;
    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, srcBuffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    uint8_t* dstBase  = reinterpret_cast<uint8_t*>(dstData);
    uint8_t* srcBase = reinterpret_cast<uint8_t*>(info.lfbPtr);
    for (FxU32 h = 0; h < srcHeight; ++h) {
        uint8_t* srcRow = srcBase + ((srcY + h) * info.strideInBytes) + (srcX * blockSize);
        uint8_t* dstRow = dstBase + (h * dstStride);
        memcpy(dstRow, srcRow, dstStride);
    }

    if (!glideDevice->LinearBufferUnlock(GR_LFB_READ_ONLY, srcBuffer))
      return FXFALSE;

    return FXTRUE;
  }

  DLLEXPORT FxBool __stdcall grLfbUnlock(GrLock_t type, GrBuffer_t buffer) {
    DEBUGLOG(">>> grLfbUnlock");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    return glideDevice->LinearBufferUnlock(type, buffer);
  }

  DLLEXPORT void __stdcall grLfbWriteColorFormat(GrColorFormat_t colorFormat) {
    DEBUGLOG("stub: grLfbWriteColorFormat");
  }

  DLLEXPORT void __stdcall grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords) {
    DEBUGLOG("stub: grLfbWriteColorSwizzle");
  }

  DLLEXPORT FxBool __stdcall grLfbWriteRegion(GrBuffer_t dstBuffer, FxU32 dstX, FxU32 dstY, GrLfbSrcFmt_t srcFormat, FxU32 srcWidth, FxU32 srcHeight,  FxBool pixelPipeline, FxI32 srcStride, void *srcData) {
    DEBUGLOG(">>> grLfbWriteRegion");

    if (unlikely(glideDevice == nullptr || srcData == nullptr || srcStride == 0))
      return FXFALSE;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);

    GrIntFmt_t format = UtilLFBSrcFormatToIFormat(srcFormat);
    if (!glideDevice->LinearBufferLock(GR_LFB_WRITE_ONLY, dstBuffer, UtilIFormatToLFBWriteMode(format), GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    FxU32 blockSize = 0;
    UtilTextureFormatSize(format, &blockSize);
    blockSize >>= 3;
    if (unlikely(blockSize == 0))
      return FXFALSE;

    uint8_t* dstBase = reinterpret_cast<uint8_t*>(info.lfbPtr);
    uint8_t* srcBase  = reinterpret_cast<uint8_t*>(srcData);
    for (FxU32 h = 0; h < srcHeight; ++h) {
        uint8_t* dstRow = dstBase + ((dstY + h) * info.strideInBytes) + (dstX * blockSize);
        uint8_t* srcRow = srcBase + (h * srcStride);
        memcpy(dstRow, srcRow, srcWidth * blockSize);
    }

    if (!glideDevice->LinearBufferUnlock(GR_LFB_WRITE_ONLY, dstBuffer))
      return FXFALSE;

    return FXTRUE;
  }

  DLLEXPORT void __stdcall grLoadGammaTable(FxU32 nentries, FxU32 *red, FxU32 *green, FxU32 *blue) {
    DEBUGLOG("stub: grLoadGammaTable");

    if (unlikely(glideDevice == nullptr || nentries == 0 || red == nullptr || green == nullptr || blue == nullptr))
      return;

    // TODO: implement proper gamma table on swapchain
  }

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

  DLLEXPORT FxBool __stdcall grReset(glide3x::GrGetReset_t pname) {
    DEBUGLOG("stub: grReset");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall grSelectContext(GrContext_t context) {
    DEBUGLOG("stub: grSelectContext");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall grSetNumPendingBuffers(FxI32 NumPendingBuffers) {
    DEBUGLOG("stub: grSetNumPendingBuffers");
  }

  DLLEXPORT void __stdcall grSplash(float x, float y, float width, float height, FxU32 frame) {
    DEBUGLOG("stub: grSplash");
  }

  DLLEXPORT void __stdcall grSstConfigPipeline(GrChipID_t chip, GrSstRegister reg, FxU32 value) {
    DEBUGLOG("stub: grSstConfigPipeline");
  }

  DLLEXPORT void __stdcall grSstOrigin(GrOriginLocation_t origin) {
    DEBUGLOG("stub: grSstOrigin");
  }

  DLLEXPORT void __stdcall grSstSelect(FxI32 sst) {
    DEBUGLOG(">>> grSstSelect");

    // We are emulating a single card
    if (unlikely(sst != 0)) {
      Logger::warn(str::format("Unexpected hwcard requested: ", sst));
    }
  }

  DLLEXPORT void __stdcall grSstVidMode(FxU32 whichSst, FxVideoTimingInfo *vidTimings) {
    DEBUGLOG("stub: grSstVidMode");
  }

  DLLEXPORT FxBool __stdcall grSstWinClose(GrContext_t context) {
    DEBUGLOG(">>> grSstWinClose");

    if (context == 1 && glideDevice != nullptr) {
      glideDevice->WindowClose();
      return FXTRUE;
    }

    return FXFALSE;
  }

  DLLEXPORT GrContext_t __stdcall grSstWinOpen(FxU32 hWnd,
                                               GrScreenResolution_t resolution,
                                               GrScreenRefresh_t refreshRate,
                                               GrColorFormat_t colorFormat,
                                               GrOriginLocation_t originLocation,
                                               FxI32 nColBuffers,
                                               FxI32 nAuxBuffers) {
    DEBUGLOG(">>> grSstWinOpen");

    if (unlikely(glideDevice == nullptr))
      return 0;

    HWND m_hWnd = reinterpret_cast<HWND>(hWnd);
    if (m_hWnd == nullptr)
      m_hWnd = GetActiveWindow();
    SetFocus(m_hWnd);

    return glideDevice->WindowOpen(m_hWnd, resolution, refreshRate, colorFormat, originLocation, nColBuffers, nAuxBuffers);
  }

  DLLEXPORT void __stdcall grStippleMode(glide3x::GrStippleMode_t mode) {
    DEBUGLOG(">>> grStippleMode");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetStippleMode(mode);
  }

  DLLEXPORT void __stdcall grStipplePattern(glide3x::GrStipplePattern_t pattern) {
    DEBUGLOG(">>> grStipplePattern");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetStipplePattern(pattern);
  }

  DLLEXPORT FxU32 __stdcall grTexCalcMemRequired(glide3x::GrLOD_t lodmin, glide3x::GrLOD_t lodmax, glide3x::GrAspectRatio_t aspect, GrTextureFormat_t fmt) {
    DEBUGLOG(">>> grTexCalcMemRequired");

    return UtilGetTextureSize(lodmin, lodmax, aspect, fmt);
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

  DLLEXPORT void __stdcall grTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, FxFloat detail_max) {
    DEBUGLOG(">>> grTexDetailControl");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexDetailControl(tmu, lod_bias, detail_scale, detail_max);
  }

  DLLEXPORT void __stdcall grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    DEBUGLOG(">>> grTexDownloadMipMap");

    if (unlikely(glideDevice == nullptr || info == nullptr || info->data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = info->smallLodLog2;
    m.largeLodLog2 = info->largeLodLog2;
    m.aspectRatioLog2 = info->aspectRatioLog2;
    m.format = info->format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(info->data);
    TMU->InsertTextures(std::move(m));
  }

  DLLEXPORT void __stdcall grTexDownloadMipMapLevel(GrChipID_t tmu, FxU32 startAddress, glide3x::GrLOD_t thisLod, glide3x::GrLOD_t largeLod, glide3x::GrAspectRatio_t aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data) {
    DEBUGLOG(">>> grTexDownloadMipMapLevel");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = thisLod;
    m.largeLodLog2 = largeLod;
    m.aspectRatioLog2 = aspectRatio;
    m.format = format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(data);
    TMU->InsertTextureLevel(std::move(m));
  }

  DLLEXPORT FxBool __stdcall grTexDownloadMipMapLevelPartial(GrChipID_t tmu, FxU32 startAddress, glide3x::GrLOD_t thisLod, glide3x::GrLOD_t largeLod, glide3x::GrAspectRatio_t aspectRatio, GrTextureFormat_t format, MipMapLevelMask_t evenOdd, void *data, FxI32 start, FxI32 end) {
    DEBUGLOG(">>> grTexDownloadMipMapLevelPartial");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return FXFALSE;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(tmu);
    if (unlikely(TMU == nullptr))
      return FXFALSE;

    GlideTMU::TMUMetadata m;
    m.startAddress = startAddress;
    m.smallLodLog2 = thisLod;
    m.largeLodLog2 = largeLod;
    m.aspectRatioLog2 = aspectRatio;
    m.format = format;
    m.evenOdd = evenOdd;
    m.data = static_cast<uint8_t*>(data);
    TMU->InsertTextureLevel(std::move(m), start, end);

    return FXTRUE;
  }

  DLLEXPORT void __stdcall grTexDownloadTable(GrTexTable_t type, void *data) {
    DEBUGLOG(">>> grTexDownloadTable");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(GR_TMU0);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, 0, 0);

    TMU = glideDevice->GetTMU(GR_TMU1);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, 0, 0);

    TMU = glideDevice->GetTMU(GR_TMU2);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, 0, 0);

    TMU = glideDevice->GetTMU(GR_TMU3);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, 0, 0);
  }

  DLLEXPORT void __stdcall grTexDownloadTablePartial(GrTexTable_t type, void * data, FxI32 start, FxI32 end) {
    DEBUGLOG(">>> grTexDownloadTablePartial");

    if (unlikely(glideDevice == nullptr || data == nullptr))
      return;

    Rc<GlideTMU> TMU = glideDevice->GetTMU(GR_TMU0);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, start, end);

    TMU = glideDevice->GetTMU(GR_TMU1);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, start, end);

    TMU = glideDevice->GetTMU(GR_TMU2);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, start, end);

    TMU = glideDevice->GetTMU(GR_TMU3);
    if (unlikely(TMU != nullptr))
      TMU->SetTable(type, data, start, end);
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

  DLLEXPORT void __stdcall grTexMultibaseAddress(GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    DEBUGLOG("stub: grTexMultibaseAddress");
  }

  DLLEXPORT void __stdcall grTexNCCTable(GrNCCTable_t table) {
    DEBUGLOG("stub: grTexNCCTable");
  }

  DLLEXPORT void __stdcall grTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo* info) {
    DEBUGLOG(">>> grTexSource");

    if (info == nullptr)
      return;

    glideDevice->SetTexSource(tmu, startAddress, evenOdd, info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format);
  }

  DLLEXPORT FxU32 __stdcall grTexTextureMemRequired(MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    DEBUGLOG(">>> grTexTextureMemRequired");

    if (info == nullptr)
      return 0;

    return UtilGetTextureSize(info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format, evenOdd);
  }

  DLLEXPORT void __stdcall grVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutOffset_t offset, glide3x::GrVertexLayoutMode_t mode) {
    DEBUGLOG(str::format(">>> grVertexLayout", param));

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetVertexLayout(param, mode, offset);
  }

  DLLEXPORT void __stdcall grViewport(FxI32 x, FxI32 y, FxI32 width, FxI32 height) {
    DEBUGLOG(">>> grViewport");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetViewport(x, y, width, height);
  }

  DLLEXPORT FxBool __stdcall gu3dfGetInfo(const char *filename, glide3x::Gu3dfInfo *info) {
    DEBUGLOG("stub: gu3dfGetInfo");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall gu3dfLoad(const char *filename, glide3x::Gu3dfInfo *data) {
    DEBUGLOG("stub: gu3dfLoad");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guFogGenerateExp(GrFog_t *fogTable, FxFloat density) {
    DEBUGLOG(">>> guFogGenerateExp");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateExp2(GrFog_t *fogTable, FxFloat density) {
    DEBUGLOG(">>> guFogGenerateExp2");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp2(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateLinear(GrFog_t *fogTable, FxFloat nearZ, FxFloat farZ) {
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

  DLLEXPORT void __stdcall guGammaCorrectionRGB(FxFloat red, FxFloat green, FxFloat blue) {
    DEBUGLOG(">>> guGammaCorrectionRGB");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetGammaCorrection(red, green, blue);
  }

  DLLEXPORT GrProc __stdcall grGetProcAddress(char *procName) {
    DEBUGLOG(">>> grGetProcAddress");

    if (procName == nullptr)
      return nullptr;
    else if (strcmp(procName, "grChromaRangeExt"))
      return reinterpret_cast<GrProc>(grChromaRangeExt);
    else if (strcmp(procName, "grChromaRangeModeExt"))
      return reinterpret_cast<GrProc>(grChromaRangeModeExt);
    else if (strcmp(procName, "grTexChromaModeExt"))
      return reinterpret_cast<GrProc>(grTexChromaModeExt);
    else if (strcmp(procName, "grTexChromaRangeExt"))
      return reinterpret_cast<GrProc>(grTexChromaRangeExt);

    Logger::err(str::format("Unsupported extension: ", procName));
    return nullptr;
  }

  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
      case DLL_PROCESS_ATTACH:
        Logger::info(">>>>>>> LOADING GXVK >>>>>>>");
        break;
      case DLL_PROCESS_DETACH: {
        Logger::info("<<<<<<< UNLOADING GXVK <<<<<<<");
        break;
      }
      default:
        break;
    }
    return FXTRUE;
  }
}
