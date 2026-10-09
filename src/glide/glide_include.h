#pragma once

#include "glide_include_1x.h"
#include "glide_include_2x.h"
#include "glide_include_3x.h"

//for some reason we need to specify __declspec(dllexport) for MinGW
#if defined(__WINE__) || !defined(_WIN32)
  #define DLLEXPORT __attribute__((visibility("default")))
#else
  #define DLLEXPORT extern
#endif

#include "../util/com/com_guid.h"
#include "../util/com/com_object.h"
#include "../util/com/com_pointer.h"

#include "../util/log/log.h"
#include "../util/log/log_debug.h"

#include "../util/sync/sync_recursive.h"

#include "../util/util_error.h"
#include "../util/util_likely.h"
#include "../util/util_string.h"

namespace dxvk {

struct GlideBoardConfiguration {
  std::string name;
  GrSstType type;
  FxU32 fbRam;
  FxU32 fbRev;
  FxI32 tmuCount;
  FxU32 tmuRam;
  FxU32 tmuRev;
  FxU32 maxTextureSize;
  FxBool uma;
  FxBool sli;
  FxBool passthrough;
};

struct GlideResolution {
  GrScreenResolution_t resolution;
  FxU32 width;
  FxU32 height;
};

struct GlideRefreshRate {
  GrScreenRefresh_t refreshRate;
  FxU32 number;
};

[[maybe_unused]]
static std::array<GlideResolution, GR_RESOLUTION_MAX + 1> GlideStaticResolutions = {{
  { GR_RESOLUTION_320x200, 320, 200 },
  { GR_RESOLUTION_320x240, 320, 240 },
  { GR_RESOLUTION_400x256, 400, 256 },
  { GR_RESOLUTION_512x384, 512, 384 },
  { GR_RESOLUTION_640x200, 640, 200 },
  { GR_RESOLUTION_640x350, 640, 350 },
  { GR_RESOLUTION_640x400, 640, 400 },
  { GR_RESOLUTION_640x480, 640, 480 },
  { GR_RESOLUTION_800x600, 800, 600 },
  { GR_RESOLUTION_960x720, 960, 720 },
  { GR_RESOLUTION_856x480, 856, 480 },
  { GR_RESOLUTION_512x256, 512, 256 },
  { GR_RESOLUTION_1024x768, 1024, 768 },
  { GR_RESOLUTION_1280x1024, 1280, 1024 },
  { GR_RESOLUTION_1600x1200, 1600, 1200 },
  { GR_RESOLUTION_400x300, 400, 300 },
  { GR_RESOLUTION_1152x864, 1152, 864 },
  { GR_RESOLUTION_1280x960, 1280, 960 },
  { GR_RESOLUTION_1600x1024, 1600, 1024 },
  { GR_RESOLUTION_1792x1344, 1792, 1344 },
  { GR_RESOLUTION_1856x1392, 1856, 1392 },
  { GR_RESOLUTION_1920x1440, 1920, 1440 },
  { GR_RESOLUTION_2048x1536, 2048, 1536 },
  { GR_RESOLUTION_2048x2048, 2048, 2048 },
}};

[[maybe_unused]]
static std::array<GlideRefreshRate, 10> GlideStaticRefreshRates = {{
  { GR_REFRESH_60Hz, 60 },
  { GR_REFRESH_70Hz, 70 },
  { GR_REFRESH_72Hz, 72 },
  { GR_REFRESH_75Hz, 75 },
  { GR_REFRESH_80Hz, 80 },
  { GR_REFRESH_90Hz, 90 },
  { GR_REFRESH_100Hz, 100 },
  { GR_REFRESH_85Hz, 85 },
  { GR_REFRESH_120Hz, 120 },
  { GR_REFRESH_NONE, 0 },
}};

static std::array<GlideBoardConfiguration, 8> GlideBoards = {{
  { "Voodoo Graphics", GR_SSTTYPE_Voodoo, 4, 1, 2, 4, 1, 256, FXFALSE, FXFALSE, FXTRUE },
  { "Voodoo Rush", GR_SSTTYPE_SST96, 4, 1, 1, 4, 1, 256, FXFALSE, FXFALSE, FXTRUE },
  { "Voodoo 2", GR_SSTTYPE_Voodoo2, 4, 1, 2, 4, 1, 256, FXFALSE, FXFALSE, FXTRUE },
  { "Voodoo Banshee", GR_SSTTYPE_Banshee, 16, 1, 1, 16, 1, 2048, FXTRUE, FXFALSE, FXFALSE },
  { "Voodoo 3 3000", GR_SSTTYPE_Voodoo3, 16, 1, 2, 16, 1, 2048, FXTRUE, FXFALSE, FXFALSE },
  { "Voodoo 4 4500", GR_SSTTYPE_Voodoo4, 32, 1, 2, 16, 1, 2048, FXTRUE, FXFALSE, FXFALSE },
  { "Voodoo 5 6000", GR_SSTTYPE_Voodoo5, 64, 1, 4, 32, 1, 2048, FXTRUE, FXFALSE, FXFALSE },
}};

enum class GLIDEAPI : uint32_t {
  API_GLIDE_1X,
  API_GLIDE_2X,
  API_GLIDE_3X
};

enum GrIntFmt_t : uint32_t {
  GR_FMT_UNKNOWN = 0,
  GR_FMT_RGB_332,
  GR_FMT_RGB_555,
  GR_FMT_RGB_565,
  GR_FMT_RGB_888,
  GR_FMT_ARGB_1555,
  GR_FMT_ARGB_4444,
  GR_FMT_ARGB_8888,
  GR_FMT_ARGB_8332,
  GR_FMT_YIQ_422,
  GR_FMT_AYIQ_8422,
  GR_FMT_A_8,
  GR_FMT_I_8,
  GR_FMT_P_8,
  GR_FMT_AI_44,
  GR_FMT_AI_88,
  GR_FMT_AP_88,
  GR_FMT_ZA16,
  GR_FMT_RLE16,
  GR_FMT_ARGB_CMP_FXT1,
  GR_FMT_YUYV_422,
  GR_FMT_UYVY_422,
  GR_FMT_AYUV_444,
  GR_FMT_ARGB_CMP_DXT1,
  GR_FMT_ARGB_CMP_DXT2,
  GR_FMT_ARGB_CMP_DXT3,
  GR_FMT_ARGB_CMP_DXT4,
  GR_FMT_ARGB_CMP_DXT5,

  GR_FMT_P_8_6666,

  // for LFB operations on depth
  GR_FMT_RGB_555_DEPTH,
  GR_FMT_RGB_565_DEPTH,
  GR_FMT_ARGB_1555_DEPTH,
};

enum class GlideShaderType : FxU8 {
  PixelShader  = 0,
  VertexShader = 1,
};

}
