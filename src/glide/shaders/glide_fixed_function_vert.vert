#version 450
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_spirv_intrinsics : require

#include "glide_fixed_function_common.glsl"

layout(location = 0) in vec2 in_XY;
layout(location = 1) in float in_Z;
layout(location = 2) in float in_W;
layout(location = 3) in float in_Q;
layout(location = 4) in float in_A;
layout(location = 5) in vec3 in_RGB;
layout(location = 6) in uvec4 in_PARGB;
layout(location = 7) in vec2 in_ST0;
layout(location = 8) in vec2 in_ST1;
layout(location = 9) in vec2 in_ST2;
layout(location = 10) in float in_Q0;
layout(location = 11) in float in_Q1;
layout(location = 12) in float in_Q2;
layout(location = 13) in float in_FOGEXT;

invariant gl_Position;

layout(location = 0) out vec3 out_STQ0;
layout(location = 1) out vec3 out_STQ1;
layout(location = 2) out vec3 out_STQ2;
layout(location = 3) out vec4 out_Color;
layout(location = 4) out float out_Z;
layout(location = 5) out float out_W;
layout(location = 6) out float out_Q;

struct GlideFixedFunctionVS {
    uint padding;
};

layout(set = CBV_SET, binding = CBV_VS_FIXED_FUNCTION, scalar, row_major)
uniform ShaderData {
    GlideFixedFunctionVS data;
};

layout(push_constant, scalar, row_major)
uniform RenderStates {
    GlideSharedPushData global;

    layout(offset = MaxSharedPushDataSize)
    GlideVsPushData vs;
    GlideFfvsPushData ffvs;
};

bool vertexHasQ0() {
  return bitfieldExtract(global.flags, 1, 1) != 0;
}

bool vertexHasST0() {
  return bitfieldExtract(global.flags, 2, 1) != 0;
}

bool vertexHasQ1() {
  return bitfieldExtract(global.flags, 3, 1) != 0;
}

bool vertexHasST1() {
  return bitfieldExtract(global.flags, 4, 1) != 0;
}

bool vertexHasQ2() {
  return bitfieldExtract(global.flags, 5, 1) != 0;
}

bool vertexHasST2() {
  return bitfieldExtract(global.flags, 6, 1) != 0;
}

bool vertexHasRGB() {
  return bitfieldExtract(global.flags, 7, 1) != 0;
}

bool vertexHasW() {
  return bitfieldExtract(global.flags, 8, 1) != 0;
}

bool vertexHasQ() {
  return bitfieldExtract(global.flags, 9, 1) != 0;
}

bool vertexHasZ() {
  return bitfieldExtract(global.flags, 10, 1) != 0;
}

bool glide3x() {
  return bitfieldExtract(global.flags, 31, 1) != 0;
}

vec3 selectSTW(uint tmu) {
  vec2 st;
  float q;

  if (tmu == 0) {
    st = in_ST0;

    if (vertexHasQ0())
      q = in_Q0;
    else
      q = in_Q;
  } else if (tmu == 1 ) {
    if (vertexHasST1())
      st = in_ST1;
    else
      st = in_ST0;

    if (vertexHasQ1())
      q = in_Q1;
    else if (vertexHasQ0())
      q = in_Q0;
    else
      q = in_Q;
  } else {
    if (vertexHasST2())
      st = in_ST2;
    else if (vertexHasST1())
      st = in_ST1;
    else
      st = in_ST0;

    if (vertexHasQ2())
      q = in_Q2;
    else if (vertexHasQ1())
      q = in_Q1;
    else if (vertexHasQ0())
      q = in_Q0;
    else
      q = in_Q;
  }

  return vec3(st, q);
}

vec4 transformVertex() {
    vec4 result;

    result = vec4(in_XY, 0.0, 1.0);
    result.xy = in_XY;

    if (all(greaterThanEqual(result.xy, vec2(GLIDE_FPBIAS)))) {
      result.xy -= GLIDE_FPBIAS;
    }

    if (global.coordinateSpace == GR_WINDOW_COORDS) {
      result.xy = vec2(result.xy) / vec2(global.viewport.extent) * 2.0 - 1.0;
    }

    return result;
}

void main() {
    gl_Position = transformVertex();
    gl_PointSize = 1.0;

    out_STQ0 = selectSTW(0);
    out_STQ1 = selectSTW(1);
    out_STQ2 = selectSTW(2);
    out_Z = vertexHasZ() && !isinf(in_Z) && !isnan(in_Z) ? in_Z : 0.0;
    out_W = vertexHasW() && !isinf(in_W) && !isnan(in_W) ? in_W : 0.0;
    out_Q = vertexHasQ() && !isinf(in_Q) && !isnan(in_Q) ? in_Q : 0.0;

    out_Color = vec4(1.0);
    if (vertexHasRGB() && !(any(isnan(in_RGB))) && !(isnan(in_A))) {
      out_Color = vec4(in_RGB / GLIDE_COLOR_MAX, in_A / GLIDE_COLOR_MAX);
    } else if (!any(isnan(in_PARGB))) {
      out_Color = swizzleColor(GR_COLORFORMAT_BGRA, in_PARGB) / GLIDE_COLOR_MAX;
    }

    out_Color = clamp(out_Color, 0.0, 1.0);
}
