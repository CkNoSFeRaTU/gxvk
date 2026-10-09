
#pragma once

#include "glide_include.h"

#include <array>
#include <bitset>
#include <optional>

#include "../util/util_vector.h"

namespace dxvk {

  struct GlideInputAssemblyState {
    glide3x::GrDrawVertexArrayMode_t mode;
  };

  static constexpr uint32_t SamplerStateCount = 13 + 1;
  static constexpr uint32_t SamplerCount      = GLIDE_MAX_TEXTURESTAGES * 2 + 1;
  static constexpr uint32_t FirstVSSamplerSlot = GLIDE_MAX_TEXTURESTAGES + 1;
  constexpr bool IsVSSampler(uint32_t Sampler) {
    return Sampler >= FirstVSSamplerSlot;
  }

  static constexpr uint32_t computeTextureBinding(GlideShaderType shaderType, uint32_t index) {
    auto base = (shaderType == GlideShaderType::VertexShader) ? FirstVSSamplerSlot : 0u;
    return base + index;
  }

  enum CbvIndex : uint32_t {
    VSClipPlanes            = 0u,
    VSFixedFunction         = 1u,
    VSVertexBlendData       = 2u,
    VSStaticConstants       = 3u,
    VSDynamicConstants      = 4u,
    PSShared                = 5u,
    PSStaticConstants       = 6u,

    Count
  };

  struct GlideFixedFunctionVS {
    uint32_t padding = 0u;
  };

  struct GlideVsPushData {
    static constexpr VkShaderStageFlags Stages = VK_SHADER_STAGE_VERTEX_BIT;
    static constexpr uint32_t           Offset = 0u;

    uint32_t padding = 0u;
  };

  struct GlideFfvsPushData {
    static constexpr VkShaderStageFlags Stages = VK_SHADER_STAGE_VERTEX_BIT;
    static constexpr uint32_t           Offset = sizeof(GlideVsPushData);

    uint32_t padding = 0u;
  };

  struct GlideFfpsPushData {
    static constexpr VkShaderStageFlags Stages = VK_SHADER_STAGE_FRAGMENT_BIT;
    static constexpr uint32_t           Offset = 0u;

    uint32_t flags = 0u;
    uint32_t chromaKeyLow = 0u;
    uint32_t chromaKeyHigh = 0u;
    uint32_t alphaTestFunction = 0u;
    uint32_t alphaTestReference = 0u;

    int32_t aspectRatioLog2[3] = { };
    uint32_t combinerTMU[GLIDE_MAX_TEXTURESTAGES] = {};
    uint32_t combinerPixelFx = 0u;

    uint32_t fogMode = 0u;
    uint32_t fogColor = 0u;
    uint32_t fogTable[4] = {};
    float depth[2] = {0.0f, 0.0f};
    float gammaCorrection[3] = {0.0f, 0.0f, 0.0f};

    uint32_t stipple = 0u;
    uint32_t constantColor = 0u;

    uint32_t depthMode = 0u;
    uint32_t ditherMode = 0u;
  };

  struct GlideSharedPushData {
    static constexpr VkShaderStageFlags Stages = VK_SHADER_STAGE_ALL_GRAPHICS;
    static constexpr uint32_t           Offset = 0u;

    VkRect2D viewport = {};
    uint32_t coordinateSpace = 0u;
    uint32_t flags = 0u;
  };

  static constexpr std::pair<VkShaderStageFlags, uint32_t> getTextureSlotInfo(uint32_t index) {
    // Sampler slot and binding indices match 1:1, see above
    return std::make_pair(IsVSSampler(index) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT, index);
  }

  struct GlidePushData {
    GlideSharedPushData shared;
    GlideVsPushData vs;
    GlideFfvsPushData ffvs;
    GlideFfpsPushData ffps;
  };

  struct GlideSpecData {
    uint32_t chromaMode = 0u;
  };

}