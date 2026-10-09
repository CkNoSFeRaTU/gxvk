#include "glide_shaders.h"
#include "glide_state.h"

#include "../dxvk/dxvk_hash.h"
#include "../dxvk/dxvk_shader_spirv.h"
#include "../util/util_small_vector.h"

#include <glide_fixed_function_frag.h>
#include <glide_fixed_function_vert.h>

namespace dxvk {

  GlideShaderModuleSet::GlideShaderModuleSet(GlideDevice* pDevice)
    : m_vs(buildVs())
    , m_fs(buildFs(pDevice)) {
  }

  Rc<DxvkShader> GlideShaderModuleSet::buildVs() {
    small_vector<DxvkBindingInfo, 1> bindings = {};

    auto& fixedFunctionDataBinding = bindings.emplace_back();
    fixedFunctionDataBinding.set             = CbvSet;
    fixedFunctionDataBinding.binding         = CbvIndex::VSFixedFunction;
    fixedFunctionDataBinding.resourceIndex   = CbvIndex::VSFixedFunction;
    fixedFunctionDataBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    fixedFunctionDataBinding.access          = VK_ACCESS_UNIFORM_READ_BIT;
    fixedFunctionDataBinding.flags.set(DxvkDescriptorFlag::UniformBuffer);

    auto& vertexBlendBinding = bindings.emplace_back();
    vertexBlendBinding.set             = CbvSet;
    vertexBlendBinding.binding         = CbvIndex::VSVertexBlendData;
    vertexBlendBinding.resourceIndex   = CbvIndex::VSVertexBlendData;
    vertexBlendBinding.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    vertexBlendBinding.access          = VK_ACCESS_SHADER_READ_BIT;
    vertexBlendBinding.flags.set(DxvkDescriptorFlag::UniformBuffer);

    auto& clipPlanesBinding = bindings.emplace_back();
    clipPlanesBinding.set             = CbvSet;
    clipPlanesBinding.binding         = CbvIndex::VSClipPlanes;
    clipPlanesBinding.resourceIndex   = CbvIndex::VSClipPlanes;
    clipPlanesBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    clipPlanesBinding.access          = VK_ACCESS_UNIFORM_READ_BIT;
    clipPlanesBinding.flags.set(DxvkDescriptorFlag::UniformBuffer);

    DxvkSpirvShaderCreateInfo info;
    info.bindingCount = bindings.size();
    info.bindings = bindings.data();
    info.flatShadingInputs = 0;
    info.sharedPushData = DxvkPushDataBlock(0u, sizeof(GlideSharedPushData), 4u, 0u);
    info.localPushData = DxvkPushDataBlock(VK_SHADER_STAGE_VERTEX_BIT, MaxSharedPushDataSize,
      GlideFfvsPushData::Offset + sizeof(GlideFfvsPushData), 4u, 0u);
    info.samplerHeap = DxvkShaderBinding();
    info.specDataBuffer = DxvkShaderBinding(VK_SHADER_STAGE_VERTEX_BIT, SpecDataSet, 0u);
    info.debugName = "FF VS";

    return new DxvkSpirvShader(info, glide_fixed_function_vert);
  }

  Rc<DxvkShader> GlideShaderModuleSet::buildFs(GlideDevice* pDevice) {
    small_vector<DxvkBindingInfo, 2 + GLIDE_MAX_TEXTURESTAGES> bindings = {};

    auto& sharedDataBinding = bindings.emplace_back();
    sharedDataBinding.set             = CbvSet;
    sharedDataBinding.binding         = CbvIndex::PSShared;
    sharedDataBinding.resourceIndex   = CbvIndex::PSShared;
    sharedDataBinding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sharedDataBinding.access          = VK_ACCESS_SHADER_READ_BIT;
    sharedDataBinding.flags.set(DxvkDescriptorFlag::UniformBuffer);

    uint32_t textureBindingId = computeTextureBinding(GlideShaderType::PixelShader, 0u);

    auto& textureBinding = bindings.emplace_back();
    textureBinding.set             = SrvSet;
    textureBinding.binding         = textureBindingId;
    textureBinding.resourceIndex   = textureBindingId;
    textureBinding.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    textureBinding.access          = VK_ACCESS_SHADER_READ_BIT;
    textureBinding.descriptorCount = GLIDE_MAX_TEXTURESTAGES;

    for (uint32_t i = 0; i < GLIDE_MAX_TEXTURESTAGES; i++) {
      uint32_t samplerBindingId = computeTextureBinding(GlideShaderType::PixelShader, i);

      auto& samplerBinding = bindings.emplace_back();
      samplerBinding.resourceIndex   = samplerBindingId;
      samplerBinding.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
      samplerBinding.blockOffset     = GetPushSamplerOffset(i);
      samplerBinding.flags.set(DxvkDescriptorFlag::PushData);
    }

    uint32_t samplerCount = GLIDE_MAX_TEXTURESTAGES;
    uint32_t samplerDwordCount = (samplerCount + 1u) / 2u;

    uint32_t pushDataSamplerOffset = GetPushSamplerOffset(0u) - MaxSharedPushDataSize;
    uint32_t pushDataSamplerShift = pushDataSamplerOffset / 4u;
    uint32_t pushDataSize = GetPushSamplerOffset(GLIDE_MAX_TEXTURESTAGES) - MaxSharedPushDataSize;

    DxvkSpirvShaderCreateInfo info;
    info.bindingCount = bindings.size();
    info.bindings = bindings.data();
    info.flatShadingInputs = 0;
    info.sharedPushData = DxvkPushDataBlock(0u, sizeof(GlideSharedPushData), 4u, 0u);
    info.localPushData = DxvkPushDataBlock(VK_SHADER_STAGE_FRAGMENT_BIT, MaxSharedPushDataSize,
      pushDataSize, 4u, ((1u << samplerDwordCount) - 1u) << pushDataSamplerShift);
    info.samplerHeap = DxvkShaderBinding(VK_SHADER_STAGE_FRAGMENT_BIT, SamplerSet, 0u);
    info.specDataBuffer = DxvkShaderBinding(VK_SHADER_STAGE_FRAGMENT_BIT, SpecDataSet, 0u);
    info.debugName = "FF FS";

    return new DxvkSpirvShader(info, glide_fixed_function_frag);
  }

}