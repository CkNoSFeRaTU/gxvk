#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_spirv_intrinsics : require
#extension GL_EXT_demote_to_helper_invocation : require
#extension GL_ARB_derivative_control : require
#extension GL_EXT_control_flow_attributes : require
#extension GL_EXT_nonuniform_qualifier : require

#include "glide_fixed_function_common.glsl"

layout(location = 0) in vec3 in_STQ0;
layout(location = 1) in vec3 in_STQ1;
layout(location = 2) in vec3 in_STQ2;
layout(location = 3) in vec4 in_Color;
layout(location = 4) in float in_Z;
layout(location = 5) in float in_W;
layout(location = 6) in float in_Q;

layout(location = 0) out vec4 out_Color;

struct GlideSharedPSStage {
    uint padding;
};

struct GlideSharedPS {
    GlideSharedPSStage Stages[TextureStageCount];
};

layout(set = CBV_SET, binding = CBV_PS_SHARED, scalar, row_major)
uniform SharedData {
    GlideSharedPS sharedData;
};

layout(push_constant, scalar, row_major)
uniform RenderStates {
    GlideSharedPushData global;

    layout(offset = MaxSharedPushDataSize)
    GlideFfpsPushData ffps;

    uint packedSamplerIndices[TextureStageCount / 2u];
};

layout(set = SRV_SET, binding = SRV_PS_BASE) uniform texture2D t2d[TextureStageCount];
layout(set = SAMPLER_SET, binding = 0) uniform sampler sampler_heap[];

#define GR_DEPTHBUFFER_DISABLE 0x00
#define GR_DEPTHBUFFER_ZBUFFER 0x01
#define GR_DEPTHBUFFER_WBUFFER 0x02
#define GR_DEPTHBUFFER_ZBUFFER_COMPARE_TO_BIAS 0x03
#define GR_DEPTHBUFFER_WBUFFER_COMPARE_TO_BIAS 0x04

#define GR_COMBINE_FUNCTION_ZERO 0x00 // GR_COMBINE_FUNCTION_NONE
#define GR_COMBINE_FUNCTION_LOCAL 0x01
#define GR_COMBINE_FUNCTION_LOCAL_ALPHA 0x02
#define GR_COMBINE_FUNCTION_SCALE_OTHER 0x03 // GR_COMBINE_FUNCTION_BLEND_OTHER
#define GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL 0x04
#define GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA 0x05
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL 0x06
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL 0x07 // GR_COMBINE_FUNCTION_BLEND
#define GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA 0x08
#define GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL 0x09 // GR_COMBINE_FUNCTION_BLEND_LOCAL
// we replace 16(0x10) with 10(0x0A) for packing reasons
#define GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA 0x0A

#define GR_COMBINE_FACTOR_ZERO 0x00 // GR_COMBINE_FACTOR_NONE
#define GR_COMBINE_FACTOR_LOCAL 0x01
#define GR_COMBINE_FACTOR_OTHER_ALPHA 0x02
#define GR_COMBINE_FACTOR_LOCAL_ALPHA 0x03
#define GR_COMBINE_FACTOR_TEXTURE_ALPHA 0x04
#define GR_COMBINE_FACTOR_TEXTURE_RGB 0x05 // GR_COMBINE_FACTOR_DETAIL_FACTOR
#define GR_COMBINE_FACTOR_LOD_FRACTION 0x05
#define GR_COMBINE_FACTOR_ONE 0x08
#define GR_COMBINE_FACTOR_ONE_MINUS_LOCAL 0x09
#define GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA 0x0A
#define GR_COMBINE_FACTOR_ONE_MINUS_LOCAL_ALPHA 0x0B
#define GR_COMBINE_FACTOR_ONE_MINUS_TEXTURE_ALPHA 0x0C // GR_COMBINE_FACTOR_ONE_MINUS_DETAIL_FACTOR
#define GR_COMBINE_FACTOR_ONE_MINUS_LOD_FRACTION 0x0D

#define GR_COMBINE_LOCAL_ITERATED 0x00
#define GR_COMBINE_LOCAL_CONSTANT 0x01 // GR_COMBINE_LOCAL_NONE
#define GR_COMBINE_LOCAL_DEPTH 0x02

#define GR_COMBINE_OTHER_ITERATED 0x00
#define GR_COMBINE_OTHER_TEXTURE 0x01
#define GR_COMBINE_OTHER_CONSTANT 0x02 // GR_COMBINE_OTHER_NONE

#define GR_ALPHASOURCE_CC_ALPHA 0x00
#define GR_ALPHASOURCE_ITERATED_ALPHA 0x01
#define GR_ALPHASOURCE_TEXTURE_ALPHA 0x02
#define GR_ALPHASOURCE_TEXTURE_ALPHA_TIMES_ITERATED_ALPHA 0x03

uint loadSamplerHeapIndex(uint samplerBindingIndex) {
  uint packedSamplerIndex = packedSamplerIndices[samplerBindingIndex / 2u];
  return bitfieldExtract(packedSamplerIndex, 16 * (int(samplerBindingIndex) & 1), 16);
}

bool getAlphaLighting() {
  return bitfieldExtract(ffps.flags, 0, 1) != 0;
}

bool getColorMask() {
  return bitfieldExtract(ffps.flags, 1, 1) != 0;
}

bool getAlphaMask() {
  return bitfieldExtract(ffps.flags, 2, 1) != 0;
}

uint getAlphaFunctionTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 0, 4);
}

uint getColorFunctionTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 4, 4);
}

uint getAlphaFactorTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 8, 4);
}

uint getColorFactorTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 12, 4);
}

bool getAlphaInvertTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 16, 1) != 0;
}

bool getColorInvertTMU(uint combinerTMU) {
  return bitfieldExtract(combinerTMU, 17, 1) != 0;
}

uint getAlphaFunction() {
  return bitfieldExtract(ffps.combinerPixelFx, 0, 4);
}

uint getColorFunction() {
  return bitfieldExtract(ffps.combinerPixelFx, 4, 4);
}

uint getAlphaFactor() {
  return bitfieldExtract(ffps.combinerPixelFx, 8, 4);
}

uint getColorFactor() {
  return bitfieldExtract(ffps.combinerPixelFx, 12, 4);
}

uint getAlphaLocal() {
  return bitfieldExtract(ffps.combinerPixelFx, 16, 2);
}

uint getColorLocal() {
  return bitfieldExtract(ffps.combinerPixelFx, 18, 2);
}

uint getAlphaOther() {
  return bitfieldExtract(ffps.combinerPixelFx, 20, 2);
}

uint getColorOther() {
  return bitfieldExtract(ffps.combinerPixelFx, 22, 2);
}

bool getAlphaInvert() {
  return bitfieldExtract(ffps.combinerPixelFx, 24, 1) != 0;
}

bool getColorInvert() {
  return bitfieldExtract(ffps.combinerPixelFx, 25, 1) != 0;
}

uint getStippleMode() {
  return bitfieldExtract(ffps.stipple, 8, 16);
}

uint getStipplePattern() {
  return bitfieldExtract(ffps.stipple, 0, 8);
}

vec2 transformTexCoord(vec3 stq, float a) {
  float aspect = exp2(a);

  vec2 extent = GLIDE_TEXCOORD * vec2(
      min(1.0, aspect),
      min(1.0, 1.0 / aspect)
  );

  return vec2(stq.xy / stq.z / extent);
}

vec4 sampleTexture(uint stage) {
  vec2 texcoord;
  if (stage == 0)
    texcoord = transformTexCoord(in_STQ0, ffps.aspectRatioLog2.x);
  else if (stage == 1)
    texcoord = transformTexCoord(in_STQ1, ffps.aspectRatioLog2.y);
  else if (stage == 2)
    texcoord = transformTexCoord(in_STQ2, ffps.aspectRatioLog2.z);

  return texture(sampler2D(t2d[stage], sampler_heap[loadSamplerHeapIndex(stage)]), texcoord);
}

float opAlphaFactor(uint operation, float localAlpha, float otherAlpha, vec4 texture) {
  if (operation == GR_COMBINE_FACTOR_LOCAL || operation == GR_COMBINE_FACTOR_LOCAL_ALPHA)
    return localAlpha;
  else if (operation == GR_COMBINE_FACTOR_OTHER_ALPHA)
    return otherAlpha;
  else if (operation == GR_COMBINE_FACTOR_TEXTURE_ALPHA)
    return texture.a;
  else if (operation == GR_COMBINE_FACTOR_ONE)
    return 1.0;
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_LOCAL || operation == GR_COMBINE_FACTOR_ONE_MINUS_LOCAL_ALPHA)
    return 1.0 - localAlpha;
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA)
    return 1.0 - otherAlpha;
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_TEXTURE_ALPHA)
    return 1.0 - texture.a;

  return 0.0;
}

float opAlphaFunction(uint operation, float factorAlpha, float localAlpha, float otherAlpha) {
  if (operation == GR_COMBINE_FUNCTION_LOCAL)
    return localAlpha;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER)
    return factorAlpha * otherAlpha;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL || operation == GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA)
    return factorAlpha * otherAlpha + localAlpha;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL)
    return factorAlpha * (otherAlpha - localAlpha);
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL || operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA)
    return factorAlpha * (otherAlpha - localAlpha) + localAlpha;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL || operation == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA)
    return factorAlpha * (-localAlpha) + localAlpha;

  return 0.0;
}

vec3 opColorFactor(uint operation, vec3 localColor, float localAlpha, float otherAlpha, vec4 texture) {
  if (operation == GR_COMBINE_FACTOR_LOCAL)
    return localColor;
  else if (operation == GR_COMBINE_FACTOR_OTHER_ALPHA)
    return vec3(otherAlpha);
  else if (operation == GR_COMBINE_FACTOR_LOCAL_ALPHA)
    return vec3(localAlpha);
  else if (operation == GR_COMBINE_FACTOR_TEXTURE_ALPHA)
    return vec3(texture.a);
  else if (operation == GR_COMBINE_FACTOR_TEXTURE_RGB)
    return texture.rgb;
  else if (operation == GR_COMBINE_FACTOR_ONE)
    return vec3(1.0);
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_LOCAL)
    return vec3(1.0) - localColor;
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA)
    return vec3(1.0 - otherAlpha);
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_LOCAL_ALPHA)
    return vec3(1.0 - localAlpha);
  else if (operation == GR_COMBINE_FACTOR_ONE_MINUS_TEXTURE_ALPHA)
    return vec3(1.0 - texture.a);

  return vec3(0.0);
}

vec3 opColorFunction(uint operation, vec3 factorColor, vec3 localColor, vec3 otherColor, float localAlpha) {
  if (operation == GR_COMBINE_FUNCTION_LOCAL)
    return localColor;
  else if (operation == GR_COMBINE_FUNCTION_LOCAL_ALPHA)
    return vec3(localAlpha);
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER)
    return factorColor * otherColor;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL)
    return factorColor * otherColor + localColor;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL_ALPHA)
    return factorColor * otherColor + vec3(localAlpha);
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL)
    return factorColor * (otherColor - localColor);
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL)
    return factorColor * (otherColor - localColor) + localColor;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_OTHER_MINUS_LOCAL_ADD_LOCAL_ALPHA)
    return factorColor * (otherColor - localColor) + vec3(localAlpha);
  else if (operation == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL)
    return factorColor * (-localColor) + localColor;
  else if (operation == GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA)
    return factorColor * (-localColor) + vec3(localAlpha);

  return vec3(0.0);
}

vec4 combinerTMU(uint stage, vec4 localInput, vec4 otherInput) {
  const uint factorColor = getColorFactorTMU(ffps.combinerTMU[stage]);
  const uint factorAlpha = getAlphaFactorTMU(ffps.combinerTMU[stage]);
  const uint functionColor = getColorFunctionTMU(ffps.combinerTMU[stage]);
  const uint functionAlpha = getAlphaFunctionTMU(ffps.combinerTMU[stage]);
  const bool invertColor = getColorInvertTMU(ffps.combinerTMU[stage]);
  const bool invertAlpha = getAlphaInvertTMU(ffps.combinerTMU[stage]);

  vec4 result = vec4(0.0);

  const float factorA = opAlphaFactor(factorAlpha, localInput.a, otherInput.a, localInput);
  result.a = opAlphaFunction(functionAlpha, factorA, localInput.a, otherInput.a);

  if (invertAlpha)
    result.a = 1.0 - result.a;

  const vec3 factorC = opColorFactor(factorColor, localInput.rgb, localInput.a, otherInput.a, localInput);
  result.rgb = opColorFunction(functionColor, factorC, localInput.rgb, otherInput.rgb, localInput.a);

  if (invertColor)
    result.rgb = 1.0 - result.rgb;

  return result;
}

vec4 combinerPixelFx(vec4 vertColor, vec4 texColor) {
  vec4 result = vec4(0.0);
  vec4 constantColor = decodeColor(getColorFormat(global.flags), ffps.constantColor);

  float localAlpha = 0.0;
  uint curAlphaLocal = getAlphaLocal();
  if (curAlphaLocal == GR_COMBINE_LOCAL_ITERATED)
    localAlpha = vertColor.a;
  else if (curAlphaLocal == GR_COMBINE_LOCAL_CONSTANT)
    localAlpha = constantColor.a;

  float otherAlpha = 0.0;
  uint curAlphaOther = getAlphaOther();
  if (curAlphaOther == GR_COMBINE_OTHER_ITERATED)
    otherAlpha = vertColor.a;
  else if (curAlphaOther == GR_COMBINE_OTHER_TEXTURE)
    otherAlpha = texColor.a;
  else if (curAlphaOther == GR_COMBINE_OTHER_CONSTANT)
    otherAlpha = constantColor.a;

  float factorAlpha = opAlphaFactor(getAlphaFactor(), localAlpha, otherAlpha, texColor);
  result.a = opAlphaFunction(getAlphaFunction(), factorAlpha, localAlpha, otherAlpha);
  if (getAlphaInvert())
    result.a = 1.0 - result.a;

  vec3 localColor = vec3(0.0);
  if (getAlphaLighting()) {
    localColor = texColor.a >= 0.5f ? constantColor.rgb : vertColor.rgb;
  } else {
    uint curColorLocal = getColorLocal();
    if (curColorLocal == GR_COMBINE_LOCAL_ITERATED)
      localColor = vertColor.rgb;
    else if (curColorLocal == GR_COMBINE_LOCAL_CONSTANT)
      localColor = constantColor.rgb;
    // else if (curColorLocal == GR_COMBINE_LOCAL_DEPTH)
  }

  vec3 otherColor = vec3(0.0);
  uint curColorOther = getColorOther();
  if (curColorOther == GR_COMBINE_OTHER_ITERATED)
    otherColor = vertColor.rgb;
  else if (curColorOther == GR_COMBINE_OTHER_TEXTURE)
    otherColor = texColor.rgb;
  else if (curColorOther == GR_COMBINE_OTHER_CONSTANT)
    otherColor = constantColor.rgb;

  vec3 factorColor = opColorFactor(getColorFactor(), localColor, localAlpha, otherAlpha, texColor);
  result.rgb = opColorFunction(getColorFunction(), factorColor, localColor, otherColor, localAlpha);
  if (getColorInvert())
    result.rgb = vec3(1.0) - result.rgb;

  return result;
}

void alphaTest(float alpha) {
  // set passed for GR_CMP_ALWAYS there and skip check
  bool passed = true;

  float alphaReference = ffps.alphaTestReference / GLIDE_COLOR_MAX;
  if (ffps.alphaTestFunction == GR_CMP_NEVER)
    passed = false;
  else if (ffps.alphaTestFunction == GR_CMP_LESS)
    passed = alpha < alphaReference;
  else if (ffps.alphaTestFunction == GR_CMP_EQUAL)
    passed = alpha == alphaReference;
  else if (ffps.alphaTestFunction == GR_CMP_LEQUAL)
    passed = alpha <= alphaReference;
  else if (ffps.alphaTestFunction == GR_CMP_GREATER)
    passed = alpha > alphaReference;
  else if (ffps.alphaTestFunction == GR_CMP_NOTEQUAL)
    passed = alpha != alphaReference;
  else if (ffps.alphaTestFunction == GR_CMP_GEQUAL)
    passed = alpha >= alphaReference;

  if (!passed)
    discard;
}

void chromaTest(vec4 textureColor) {
  if (chromaMode == GR_CHROMAKEY_ENABLE) {
    vec4 src = textureColor;
    vec4 lkey = decodeColor(getColorFormat(global.flags), ffps.chromaKeyLow);
    vec4 hkey = decodeColor(getColorFormat(global.flags), ffps.chromaKeyHigh);
    bool passed = true;
    if (!getColorMask() && (src.a >= lkey.a && src.a <= hkey.a))
      passed = false;
    else if (!getAlphaMask() && all(greaterThanEqual(src.rgb, lkey.rgb)) && all(lessThanEqual(src.rgb, hkey.rgb)))
      passed = false;
    else if (all(greaterThanEqual(src, lkey)) && all(lessThanEqual(src, hkey)))
      passed = false;

    if (!passed)
      discard;
  }
}

void main() {
  vec4 textureColor = in_Color;
  textureColor = combinerTMU(2, sampleTexture(2), textureColor);
  textureColor = combinerTMU(1, sampleTexture(1), textureColor);
  textureColor = combinerTMU(0, sampleTexture(0), textureColor);

  vec4 resultColor = combinerPixelFx(in_Color, textureColor);

  if (ffps.depthMode == GR_DEPTHBUFFER_WBUFFER || ffps.depthMode == GR_DEPTHBUFFER_WBUFFER_COMPARE_TO_BIAS) {
    float w = 1.0 / in_Q; // gl_FragCoord.w;
    gl_FragDepth = clamp((w - ffps.depth.y) / (ffps.depth.x - ffps.depth.y), 0.0, 1.0);
  } else {
    gl_FragDepth = clamp(in_Z / (ffps.depth.x - ffps.depth.y), 0.0, 1.0);
  }

  chromaTest(textureColor);
  alphaTest(resultColor.a);

  if (!getIgnoreGammaCorrection(global.flags) && all(greaterThan(ffps.gammaCorrection, vec3(0.0))) && !all(equal(ffps.gammaCorrection, vec3(1.0)))) {
    resultColor.rgb = pow(resultColor.rgb, 1.0 / ffps.gammaCorrection);
  }

  out_Color = resultColor;
}
