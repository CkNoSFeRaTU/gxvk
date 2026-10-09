#include "glide_include.h"

#include "glide_device.h"
#include "glide_options.h"
#include "glide_utils.h"

#include "../util/util_singleton.h"

namespace dxvk {
  Logger Logger::s_instance("glide3x.log");
  GlideDevice* glideDevice = nullptr;
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
  DLLEXPORT void __stdcall grAADrawTriangle(const void *a, const void *b, const void *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias) {
    Logger::debug("stub: grAADrawTriangle");
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

  DLLEXPORT void __stdcall grBufferClear(GrColor_t color, GrAlpha_t alpha, FxU32 depth) {
    Logger::debug(">>> grBufferClear");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->BufferClear(color, alpha, depth);
  }

  DLLEXPORT void __stdcall grBufferSwap(FxU32 swap_interval) {
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

  DLLEXPORT void __stdcall grConstantColorValue(GrColor_t value) {
    Logger::debug(">>> grConstantColorValue");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetConstantColor(value);
  }

  DLLEXPORT void __stdcall grCoordinateSpace(glide3x::GrCoordinateSpaceMode_t mode) {
    Logger::debug(">>> grCoordinateSpace");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCoordinateSpace(mode);
  }

  DLLEXPORT void __stdcall grCullMode(GrCullMode_t mode) {
    Logger::debug(">>> grCullMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetCullMode(mode);
  }

  DLLEXPORT void __stdcall grDepthBiasLevel(FxI32 level) {
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

  DLLEXPORT void __stdcall grDepthMask(FxBool mask) {
    Logger::debug(">>> grDepthMask");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthMask(mask);
  }

  DLLEXPORT void __stdcall grDepthRange(FxFloat n, FxFloat f) {
    Logger::debug(">>> grDepthRange");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDepthRange(n, f);
  }

  DLLEXPORT void __stdcall grDisable(GrEnableMode_t mode) {
    Logger::debug("stub: grDisable");
  }

  DLLEXPORT void __stdcall grDisableAllEffects() {
    Logger::debug("stub: grDisableAllEffects");
  }

  DLLEXPORT void __stdcall grDitherMode(GrDitherMode_t mode) {
    Logger::debug(">>> grDitherMode");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetDitherMode(mode);
  }

  DLLEXPORT void __stdcall grDrawLine(const void *v1, const void *v2) {
    Logger::debug(">>> grDrawLine");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawLine(v1, v2);
  }

  DLLEXPORT void __stdcall grDrawPoint(const void *pt) {
    Logger::debug(">>> grDrawPoint");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawPoint(pt);
  }

  DLLEXPORT void __stdcall grDrawTriangle(const void *a, const void *b, const void *c) {
    Logger::debug(">>> grDrawTriangle");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawTriangle(a, b, c);
  }

  DLLEXPORT void __stdcall grDrawVertexArray(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, void *pointers) {
    Logger::debug(">>> grDrawVertexArray");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArray(mode, count, pointers);
  }

  DLLEXPORT void __stdcall grDrawVertexArrayContiguous(glide3x::GrDrawVertexArrayMode_t mode, FxU32 count, void *pointers, FxU32 stride) {
    Logger::debug(">>> grDrawVertexArrayContiguous");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->DrawVertexArrayContiguous(mode, count, pointers, stride);
  }

  DLLEXPORT void __stdcall grEnable(GrEnableMode_t mode) {
    Logger::debug("stub: grEnable");
  }

  DLLEXPORT void __stdcall grErrorSetCallback(GrErrorCallbackFnc_t fnc) {
    Logger::debug("stub: grErrorSetCallback");
  }

  DLLEXPORT void __stdcall grFinish() {
    Logger::debug("stub: grFinish");
  }

  DLLEXPORT void __stdcall grFlush() {
    Logger::debug(">>> grFlush");

    if (unlikely(glideDevice == nullptr))
      return;

    return glideDevice->Flush();
  }

  DLLEXPORT void __stdcall grFogColorValue(GrColor_t fogcolor) {
    Logger::debug("stub: grFogColorValue");
  }

  DLLEXPORT void __stdcall grFogMode(glide3x::GrFogMode_t mode) {
    Logger::debug("stub: grFogMode");
  }

  DLLEXPORT void __stdcall grFogTable(const GrFog_t *ft) {
    Logger::debug("stub: grFogTable");
  }

  DLLEXPORT FxU32 __stdcall grGet(glide3x::GrGetReset_t pname, FxU32 plength, FxI32 *params) {
    Logger::debug(str::format(">>> grGet: ", pname));

    switch (pname) {
      case(glide3x::GrGetReset_t::GR_BITS_GAMMA):
        if (plength >= 4)
          params[0] = 8;
        break;
      case(glide3x::GrGetReset_t::GR_BITS_DEPTH):
        if (plength >= 4)
          params[0] = 16;
        break;
      case(glide3x::GrGetReset_t::GR_BITS_RGBA):
        if (plength >= 4)
          params[0] = 32;
        break;
      case(glide3x::GrGetReset_t::GR_FOG_TABLE_ENTRIES):
        if (plength >= 4)
          params[0] = FOG_TABLE_ENTRIES_COUNT;
        break;
      case(glide3x::GrGetReset_t::GR_GAMMA_TABLE_ENTRIES):
        if (plength >= 4)
          params[0] = GAMMA_TABLE_ENTRIES_COUNT;
        break;
      case(glide3x::GrGetReset_t::GR_GLIDE_STATE_SIZE):
        if (plength >= 4) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GlideStateSize();
          else
            params[0] = 0;
        }
        break;
      case(glide3x::GrGetReset_t::GR_GLIDE_VERTEXLAYOUT_SIZE):
        if (plength >= 4) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GetVertexLayoutSize();
          else
            params[0] = 0;
        }
        break;
      case(glide3x::GrGetReset_t::GR_IS_BUSY):
        if (plength >= 4)
          params[0] = 0; // we are never busy
        break;
      case(glide3x::GrGetReset_t::GR_MAX_TEXTURE_ASPECT_RATIO):
        if (plength >= 4)
          params[0] = glide3x::GR_ASPECT_LOG2_8x1;
        break;
      case(glide3x::GrGetReset_t::GR_MAX_TEXTURE_SIZE):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->maxTextureSize * OneMB;
          } else {
            params[0] = 0;
          }
        }
        break;
      case(glide3x::GrGetReset_t::GR_MEMORY_FB):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            if (boardConfig->uma)
              params[0] = 0;
            else
              params[0] = boardConfig->fbRam * OneMB;
          }
        }
        break;
      case(glide3x::GrGetReset_t::GR_MEMORY_TMU):
        if (plength >= 4) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GetTMUMemory() * OneMB;
          else
            params[0] = 0;
        }
        break;
      case(glide3x::GrGetReset_t::GR_MEMORY_UMA):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
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
      case(glide3x::GrGetReset_t::GR_NUM_BOARDS):
        if (plength >= 4) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GetHWConfiguration()->num_sst;
          else
            params[0] = 1;
        }
        break;
      case(glide3x::GrGetReset_t::GR_NUM_FB):
        if (plength >= 4)
          params[0] = 1;
        break;
      case(glide3x::GrGetReset_t::GR_NUM_TMU):
        if (plength >= 4) {
          if (glideDevice != nullptr)
            params[0] = glideDevice->GetTMUCount();
          else
            params[0] = 0;
        }
        break;
      case(glide3x::GrGetReset_t::GR_PENDING_BUFFERSWAPS):
        if (plength >= 4)
          params[0] = 0; // we are never busy
        break;
      case(glide3x::GrGetReset_t::GR_REVISION_FB):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->fbRev;
          } else {
            params[0] = 0;
          }
        }
        break;
      case(glide3x::GrGetReset_t::GR_REVISION_TMU):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->tmuRev;
          } else {
            params[0] = 0;
          }
        }
        break;
      case(glide3x::GrGetReset_t::GR_SUPPORTS_PASSTHRU):
        if (plength >= 4) {
          if (glideDevice != nullptr) {
            const GlideBoardConfiguration* boardConfig = glideDevice->GetBoardConfiguration();
            params[0] = boardConfig->passthrough;
          } else {
            params[0] = 0;
          }
        }
        break;
      case(glide3x::GrGetReset_t::GR_TEXTURE_ALIGN):
        if (plength >= 4)
          params[0] = SST1_TEXTURE_ALIGN;
        break;
      case(glide3x::GrGetReset_t::GR_VIEWPORT):
        if (plength >= 16) {
          if (glideDevice != nullptr) {
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
      case(glide3x::GrGetReset_t::GR_WDEPTH_MIN_MAX):
        if (plength >= 8) {
          params[0] = SST1_WDEPTHVALUE_NEAREST;
          params[1] = SST1_WDEPTHVALUE_FARTHEST;
        }
        break;
      case(glide3x::GrGetReset_t::GR_ZDEPTH_MIN_MAX):
        if (plength >= 8) {
          params[0] = SST1_ZDEPTHVALUE_NEAREST;
          params[1] = SST1_ZDEPTHVALUE_FARTHEST;
        }
        break;
      default:
        Logger::warn(str::format("Unknown grGet pname: ", pname));
        break;
    }

    return plength;
  }

  DLLEXPORT GrProc __stdcall grGetProcAddress(char *procName) {
    Logger::debug(">>> grGetProcAddress");

    if (procName == nullptr)
      return nullptr;

    Logger::err(str::format("Unsupported extention: ", procName));
    return nullptr;
  }

  DLLEXPORT const char* __stdcall grGetString(glide3x::GrGetParam_t pname) {
    Logger::debug(">>> grGetString");

    switch (pname) {
      case(glide3x::GrGetParam_t::GR_EXTENSION):
        return " ";
      case(glide3x::GrGetParam_t::GR_HARDWARE):
        if (glideDevice != nullptr) {
          return glideDevice->GetBoardConfiguration()->name.c_str();
        }
        return "GXVK";
      case(glide3x::GrGetParam_t::GR_RENDERER):
        return "Glide";
      case(glide3x::GrGetParam_t::GR_VENDOR):
        return "GXVK";
      case(glide3x::GrGetParam_t::GR_VERSION):
        return "3.0";
    }

    return nullptr;
  }

  DLLEXPORT void __stdcall grGlideGetState(void *state) {
    Logger::debug(">>> grGlideGetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->LoadState(state);
  }

  DLLEXPORT void __stdcall grGlideGetVertexLayout(void *layout) {
    Logger::debug(">>> grGlideGetVertexLayout");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->LoadVertexLayout(layout);
  }

  DLLEXPORT void __stdcall grGlideInit() {
    Logger::debug(">>> grGlideInit");

    initDXVKDevice();
  }

  DLLEXPORT void __stdcall grGlideSetState(const void *state) {
    Logger::debug(">>> grGlideSetState");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SaveState(state);
  }

  DLLEXPORT void __stdcall grGlideSetVertexLayout(const void *layout) {
    Logger::debug(">>> grGlideSetVertexLayout");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SaveVertexLayout(layout);
  }

  DLLEXPORT void __stdcall grGlideShutdown() {
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

  DLLEXPORT void __stdcall grLfbConstantAlpha(GrAlpha_t alpha) {
    Logger::debug("stub: grLfbConstantAlpha");
  }

  DLLEXPORT void __stdcall grLfbConstantDepth(FxU32 depth) {
    Logger::debug("stub: grLfbConstantDepth");
  }

  DLLEXPORT FxBool __stdcall grLfbLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info) {
    Logger::debug(">>> grLfbLock");

    if (unlikely(glideDevice == nullptr))
      return FXFALSE;

    return glideDevice->LinearBufferLock(type, buffer, writeMode, origin, pixelPipeline, info);
  }

  DLLEXPORT FxBool __stdcall grLfbReadRegion(GrBuffer_t buffer, FxU32 x, FxU32 y, FxU32 width, FxU32 height, FxU32 stride, void *data) {
    Logger::debug(">>> grLfbReadRegion");

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    if (!glideDevice->LinearBufferLock(GR_LFB_READ_ONLY, buffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    size_t pitch = std::min(info.strideInBytes, stride);
    uint8_t *src = static_cast<uint8_t*>(info.lfbPtr);
    uint8_t *dst = static_cast<uint8_t*>(data);
    for (FxU32 h = 0; h < height; h++) {
      memcpy(dst, src, pitch);
      src += info.strideInBytes;
      dst += stride;
    }

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

  DLLEXPORT void __stdcall grLfbWriteColorFormat(GrColorFormat_t colorFormat) {
    Logger::debug("stub: grLfbWriteColorFormat");
  }

  DLLEXPORT void __stdcall grLfbWriteColorSwizzle(FxBool swizzleBytes, FxBool swapWords) {
    Logger::debug("stub: grLfbWriteColorSwizzle");
  }

  DLLEXPORT FxBool __stdcall grLfbWriteRegion(GrBuffer_t dst_buffer, FxU32 dst_x, FxU32 dst_y, GrLfbSrcFmt_t src_format, FxU32 src_width, FxU32 src_height, FxBool pixelPipeline, FxI32 src_stride, void *src_data) {
    Logger::debug(">>> grLfbWriteRegion");

    if (unlikely(glideDevice == nullptr || src_data == nullptr))
      return FXFALSE;

    GrLfbInfo_t info;
    info.size = sizeof(GrLfbInfo_t);
    // Docs says that GR_LFBWRITEMODE_565 is assumed
    if (!glideDevice->LinearBufferLock(GR_LFB_WRITE_ONLY, dst_buffer, GR_LFBWRITEMODE_565, GR_ORIGIN_UPPER_LEFT, FXFALSE, &info))
      return FXFALSE;

    size_t pitch = std::min(static_cast<FxI32>(info.strideInBytes), src_stride);
    uint8_t *src = static_cast<uint8_t*>(src_data);
    uint8_t *dst = static_cast<uint8_t*>(info.lfbPtr);
    for (FxU32 h = 0; h < src_height; h++) {
      memcpy(dst, src, pitch);
      src += src_stride;
      dst += info.strideInBytes;
    }

    if (!glideDevice->LinearBufferUnlock(GR_LFB_WRITE_ONLY, dst_buffer))
      return FXFALSE;

    return FXTRUE;
  }

  DLLEXPORT void __stdcall grLoadGammaTable(FxU32 nentries, FxU32 *red, FxU32 *green, FxU32 *blue) {
    Logger::debug("stub: grLoadGammaTable");

    if (unlikely(glideDevice == nullptr || nentries == 0 || red == nullptr || green == nullptr || blue == nullptr))
      return;

    // TODO: implement proper table on swapchain
  }

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

  DLLEXPORT FxBool __stdcall grReset(glide3x::GrGetReset_t pname) {
    Logger::debug("stub: grReset");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall grSelectContext(GrContext_t context) {
    Logger::debug("stub: grSelectContext");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall grSetNumPendingBuffers(FxI32 NumPendingBuffers) {
    Logger::debug("stub: grSetNumPendingBuffers");
  }

  DLLEXPORT void __stdcall grSplash(float x, float y, float width, float height, FxU32 frame) {
    Logger::debug("stub: grSplash");
  }

  DLLEXPORT void __stdcall grSstConfigPipeline(GrChipID_t chip, GrSstRegister reg, FxU32 value) {
    Logger::debug("stub: grSstConfigPipeline");
  }

  DLLEXPORT void __stdcall grSstOrigin(GrOriginLocation_t origin) {
    Logger::debug("stub: grSstOrigin");
  }

  DLLEXPORT void __stdcall grSstSelect(FxI32 sst) {
    Logger::debug(">>> grSstSelect");

    // We are emulating a single card
    if (unlikely(sst != 0)) {
      Logger::warn(str::format("Unexpected hwcard requested: ", sst));
    }
  }

  DLLEXPORT void __stdcall grSstVidMode(FxU32 whichSst, FxVideoTimingInfo *vidTimings) {
    Logger::debug("stub: grSstVidMode");
  }

  DLLEXPORT FxBool __stdcall grSstWinClose(GrContext_t context) {
    Logger::debug(">>> grSstWinClose");

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
    Logger::debug(">>> grSstWinOpen");

    if (unlikely(glideDevice == nullptr))
      return 0;

    HWND m_hWnd = reinterpret_cast<HWND>(hWnd);
    if (m_hWnd == nullptr)
      m_hWnd = GetActiveWindow();
    SetFocus(m_hWnd);

    return glideDevice->WindowOpen(m_hWnd, resolution, refreshRate, colorFormat, originLocation, nColBuffers, nAuxBuffers);
  }

  DLLEXPORT void __stdcall grStippleMode(glide3x::GrStippleMode_t mode) {
    Logger::debug(">>> grStippleMode");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetStippleMode(mode);
  }

  DLLEXPORT void __stdcall grStipplePattern(glide3x::GrStipplePattern_t pattern) {
    Logger::debug(">>> grStipplePattern");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetStipplePattern(pattern);
  }

  DLLEXPORT FxU32 __stdcall grTexCalcMemRequired(glide3x::GrLOD_t lodmin, glide3x::GrLOD_t lodmax, glide3x::GrAspectRatio_t aspect, GrTextureFormat_t fmt) {
    Logger::debug(">>> grTexCalcMemRequired");

    return UtilGetTextureSize(lodmin, lodmax, aspect, fmt);
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

  DLLEXPORT void __stdcall grTexDetailControl(GrChipID_t tmu, FxI32 lod_bias, FxU8 detail_scale, FxFloat detail_max) {
    Logger::debug(">>> grTexDetailControl");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetTexDetailControl(tmu, lod_bias, detail_scale, detail_max);
  }

  DLLEXPORT void __stdcall grTexDownloadMipMap(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    Logger::debug(">>> grTexDownloadMipMap");

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
    Logger::debug(">>> grTexDownloadMipMapLevel");

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
    Logger::debug(">>> grTexDownloadMipMapLevelPartial");

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
    Logger::debug(">>> grTexDownloadTable");

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
    Logger::debug(">>> grTexDownloadTablePartial");

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

  DLLEXPORT void __stdcall grTexMultibaseAddress(GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    Logger::debug("stub: grTexMultibaseAddress");
  }

  DLLEXPORT void __stdcall grTexNCCTable(GrNCCTable_t table) {
    Logger::debug("stub: grTexNCCTable");
  }

  DLLEXPORT void __stdcall grTexSource(GrChipID_t tmu, FxU32 startAddress, MipMapLevelMask_t evenOdd, glide3x::GrTexInfo* info) {
    Logger::debug(">>> grTexSource");

    if (info == nullptr)
      return;

    glideDevice->SetTexSource(tmu, startAddress, evenOdd, info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format);
  }

  DLLEXPORT FxU32 __stdcall grTexTextureMemRequired(MipMapLevelMask_t evenOdd, glide3x::GrTexInfo *info) {
    Logger::debug(">>> grTexTextureMemRequired");

    if (info == nullptr)
      return 0;

    return UtilGetTextureSize(info->smallLodLog2, info->largeLodLog2, info->aspectRatioLog2, info->format, evenOdd);
  }

  DLLEXPORT void __stdcall grVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutOffset_t offset, glide3x::GrVertexLayoutMode_t mode) {
    Logger::debug(str::format(">>> grVertexLayout", param));

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetVertexLayout(param, mode, offset);
  }

  DLLEXPORT void __stdcall grViewport(FxI32 x, FxI32 y, FxI32 width, FxI32 height) {
    Logger::debug(">>> grViewport");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetViewport(x, y, width, height);
  }

  DLLEXPORT FxBool __stdcall gu3dfGetInfo(const char *filename, glide3x::Gu3dfInfo *info) {
    Logger::debug("stub: gu3dfGetInfo");
    return FXFALSE;
  }

  DLLEXPORT FxBool __stdcall gu3dfLoad(const char *filename, glide3x::Gu3dfInfo *data) {
    Logger::debug("stub: gu3dfLoad");
    return FXFALSE;
  }

  DLLEXPORT void __stdcall guFogGenerateExp(GrFog_t *fogTable, FxFloat density) {
    Logger::debug(">>> guFogGenerateExp");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateExp2(GrFog_t *fogTable, FxFloat density) {
    Logger::debug(">>> guFogGenerateExp2");

    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->UtilFogGenerateExp2(fogTable, density);
  }

  DLLEXPORT void __stdcall guFogGenerateLinear(GrFog_t *fogTable, FxFloat nearZ, FxFloat farZ) {
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

  DLLEXPORT void __stdcall guGammaCorrectionRGB(FxFloat red, FxFloat green, FxFloat blue) {
    Logger::debug(">>> guGammaCorrectionRGB");
    if (unlikely(glideDevice == nullptr))
      return;

    glideDevice->SetGammaCorrection(red, green, blue);
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
