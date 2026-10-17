
#pragma once

#include "glide_include.h"

#include <array>
#include <bitset>
#include <optional>

#include "../dxvk/dxvk_shader.h"
#include "../util/util_vector.h"

namespace dxvk {

  struct GlideInputAssemblyState {
    glide3x::GrDrawVertexArrayMode_t mode;
  };

  static constexpr uint32_t SamplerCount      = GLIDE_MAX_TEXTURESTAGES * 2;
  static constexpr uint32_t FirstVSSamplerSlot = GLIDE_MAX_TEXTURESTAGES + 1;
  constexpr bool IsVSSampler(uint32_t Sampler) {
    return Sampler >= FirstVSSamplerSlot;
  }

  static constexpr uint32_t computeTextureBinding(GlideShaderType shaderType, uint32_t index) {
    auto base = (shaderType == GlideShaderType::VertexShader) ? FirstVSSamplerSlot : 0u;
    return base + index;
  }

  enum CbvIndex : uint32_t {
    VSStaticConstants       = 0u,
    VSDynamicConstants      = 1u,
    PSShared                = 2u,
    PSStaticConstants       = 3u,

    Count
  };

  struct GlideSharedPS {
    uint8_t fogTable[FOG_TABLE_ENTRIES_COUNT] = {};
    FxFloat fogIndexToW[FOG_TABLE_ENTRIES_COUNT] = {};
    struct Stage {
      uint32_t combinerTMU = {};
    } Stages[GLIDE_MAX_TEXTURESTAGES];
  };

  struct GlideFfpsPushData {
    static constexpr VkShaderStageFlags Stages = VK_SHADER_STAGE_FRAGMENT_BIT;
    static constexpr uint32_t           Offset = 0u;

    uint32_t flags = 0u;
    uint32_t chromaKeyLow = 0u;
    uint32_t chromaKeyHigh = 0u;
    uint32_t alphaTest = 0u;

    int32_t aspectRatioLog2[3] = { };
    uint32_t constantColor = 0u;

    uint32_t delta0Color = 0u;
    uint32_t fogColor = 0u;
    uint32_t stipplePattern = 0u;
    uint32_t combinerPixelFx = 0u;

    float depth[2] = {0.0f, 0.0f};
    float gammaCorrection[3] = {0.0f, 0.0f, 0.0f};
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
    GlideFfpsPushData ffps;
  };

  struct GlideSpecData {
    uint32_t chromaMode = 0u;
    uint32_t fogMode = 0u;
  };

}