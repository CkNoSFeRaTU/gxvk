const uint TextureStageCount = 3;
const uint MaxSharedPushDataSize = 32;

// 3Dfx uses 786432.875 but we have explosions as Unreal games send 786432.0 - 786432.50
const float GLIDE_FPBIAS = 786432.0;
const float GLIDE_TEXCOORD = 256.0;
const float GLIDE_COLOR_MAX = 255.0;

// Compare functions
#define GR_CMP_NEVER 0x00
#define GR_CMP_LESS 0x01
#define GR_CMP_EQUAL 0x02
#define GR_CMP_LEQUAL 0x03
#define GR_CMP_GREATER 0x04
#define GR_CMP_NOTEQUAL 0x05
#define GR_CMP_GEQUAL 0x06
#define GR_CMP_ALWAYS 0x07

// Coordinate Space
#define GR_WINDOW_COORDS 0x00
#define GR_CLIP_COORDS 0x01

// Colorformats
#define GR_COLORFORMAT_ARGB 0x00
#define GR_COLORFORMAT_ABGR 0x01
#define GR_COLORFORMAT_RGBA 0x02
#define GR_COLORFORMAT_BGRA 0x03

// Chroma Key State
#define GR_CHROMAKEY_DISABLE 0x00
#define GR_CHROMAKEY_ENABLE 0x01

// Glide STWHints and GXVK flags
#define GR_STWHINT_W_DIFF_FBI 0x00
#define GR_STWHINT_W_DIFF_TMU0 0x01
#define GR_STWHINT_ST_DIFF_TMU0 0x02
#define GR_STWHINT_W_DIFF_TMU1 0x04
#define GR_STWHINT_ST_DIFF_TMU1 0x08
#define GR_STWHINT_W_DIFF_TMU2 0x10
#define GR_STWHINT_ST_DIFF_TMU2 0x20
#define GR_GXVK_RGB 0x40
#define GR_GXVK_W 0x80
#define GR_GXVK_Q 0x100
#define GR_GXVK_GLIDE3X 0x80000000

struct GlideVsPushData {
    uint padding;
};

struct GlideFfvsPushData {
    ivec3 padding;
};

struct GlideFfpsPushData {
    uint flags;
    uint chromaKeyLow;
    uint chromaKeyHigh;
    uint alphaTestFunction;
    uint alphaTestReference;

    ivec3 aspectRatioLog2;
    uvec3 combinerTMU;
    uint combinerPixelFx;

    uint fogMode;
    uint fogColor;
    uvec4 fogTable;
    vec2 depth;
    vec3 gammaCorrection;

    uint stipple;
    uint constantColor;

    uint depthMode;
    uint ditherMode;
};

struct GlideViewport {
    ivec2 offset;
    uvec2 extent;
};

struct GlideSharedPushData {
    GlideViewport viewport;
    uint coordinateSpace;
    uint flags;
};

// Thanks SPIRV-Cross
spirv_instruction(set = "GLSL.std.450", id = 79) float spvNMin(float, float);
spirv_instruction(set = "GLSL.std.450", id = 79) vec2 spvNMin(vec2, vec2);
spirv_instruction(set = "GLSL.std.450", id = 79) vec3 spvNMin(vec3, vec3);
spirv_instruction(set = "GLSL.std.450", id = 79) vec4 spvNMin(vec4, vec4);
spirv_instruction(set = "GLSL.std.450", id = 80) float spvNMax(float, float);
spirv_instruction(set = "GLSL.std.450", id = 80) vec2 spvNMax(vec2, vec2);
spirv_instruction(set = "GLSL.std.450", id = 80) vec3 spvNMax(vec3, vec3);
spirv_instruction(set = "GLSL.std.450", id = 80) vec4 spvNMax(vec4, vec4);
spirv_instruction(set = "GLSL.std.450", id = 81) float spvNClamp(float, float, float);
spirv_instruction(set = "GLSL.std.450", id = 81) vec2 spvNClamp(vec2, vec2, vec2);
spirv_instruction(set = "GLSL.std.450", id = 81) vec3 spvNClamp(vec3, vec3, vec3);
spirv_instruction(set = "GLSL.std.450", id = 81) vec4 spvNClamp(vec4, vec4, vec4);

// Bindings have to match GlideShaderResourceMapping.
// Set numbers are arbitrarily set in glide_state.h
#define SAMPLER_SET             0

#define SRV_SET                 1
#define SRV_PS_BASE             0

#define CBV_SET                 2
#define SPEC_DATA_SET           3

#define CBV_VS_CLIP_PLANES      0
#define CBV_VS_FIXED_FUNCTION   1
#define CBV_VS_VERTEX_BLEND     2

#define CBV_PS_SHARED           5

layout(set = SPEC_DATA_SET, binding = 0, scalar) uniform SpecConsts {
    uint chromaMode;
};

uvec4 swizzleColor(uint format, uvec4 color) {
  if (format == GR_COLORFORMAT_ARGB)
    return color.argb;
  else if (format == GR_COLORFORMAT_ABGR)
    return color.abgr;
  else if (format == GR_COLORFORMAT_RGBA)
    return color.rgba;
  else if (format == GR_COLORFORMAT_BGRA)
    return color.bgra;
}

vec4 decodeColor(uint format, uint color) {
  uint r, g, b, a;

  if (format == GR_COLORFORMAT_RGBA) {
    r = (color >> 24) & 0xFF;
    g = (color >> 16) & 0xFF;
    b = (color >> 8) & 0xFF;
    a = (color & 0xFF);
  }
  else if (format == GR_COLORFORMAT_ARGB) {
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = (color & 0xFF);
    a = (color >> 24) & 0xFF;
  }
  else if (format == GR_COLORFORMAT_ABGR) {
    r = (color & 0xFF);
    g = (color >> 8) & 0xFF;
    b = (color >> 16) & 0xFF;
    a = (color >> 24) & 0xFF;
  } else {
    r = (color >> 8) & 0xFF;
    g = (color >> 16) & 0xFF;
    b = (color >> 24) & 0xFF;
    a = (color & 0xFF);
  }

  return vec4(r, g, b, a) / GLIDE_COLOR_MAX;
}

uint getColorFormat(uint flags) {
  return bitfieldExtract(flags, 30, 2);
}

bool getIgnoreGammaCorrection(uint flags) {
  return bitfieldExtract(flags, 11, 1) != 0;
}
