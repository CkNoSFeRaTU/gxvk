#include "glide_texture.h"
#include "glide_device.h"

namespace dxvk {
  GlideCommonTexture::GlideCommonTexture(GlideDevice* pDevice, const GLIDE_COMMON_TEXTURE_DESC*  pDesc)
  : m_device(pDevice)
  , m_desc(*pDesc) {
    DxvkImageCreateInfo imageInfo;
    imageInfo.type            = VK_IMAGE_TYPE_2D;
    imageInfo.format          = m_desc.Format;
    imageInfo.flags           = 0;
    imageInfo.sampleCount     = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.extent.width    = m_desc.Width;
    imageInfo.extent.height   = m_desc.Height;
    imageInfo.extent.depth    = 1;
    imageInfo.numLayers       = 1;
    imageInfo.mipLevels       = m_desc.MipMaps;
    imageInfo.usage           = VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                              | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.stages          = VK_PIPELINE_STAGE_TRANSFER_BIT
                              | m_device->GetEnabledShaderStages();
    imageInfo.access          = VK_ACCESS_TRANSFER_READ_BIT
                              | VK_ACCESS_TRANSFER_WRITE_BIT
                              | VK_ACCESS_SHADER_READ_BIT;
    imageInfo.tiling          = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.layout          = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.shared          = m_desc.Type != GR_BUFFER_NONE;

    if (m_desc.Type != GR_BUFFER_NONE) {
      imageInfo.usage  |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                       |  VK_IMAGE_USAGE_SAMPLED_BIT;
      imageInfo.stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      imageInfo.access |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                       |  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      if (m_desc.Type == GR_BUFFER_BACKBUFFER) {
        std::string debugName = str::format("backbuffer ", m_desc.Id);
        imageInfo.debugName = debugName.c_str();
      }
      else if (m_desc.Type == GR_BUFFER_AUXBUFFER) {
        std::string debugName = str::format("aux ", m_desc.Id);
        imageInfo.debugName = debugName.c_str();
      }
    }

    if (m_desc.Type == GR_BUFFER_AUXBUFFER || m_desc.Type == GR_BUFFER_DEPTHBUFFER) {
      imageInfo.usage  |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                       |  VK_IMAGE_USAGE_SAMPLED_BIT;
      imageInfo.stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      imageInfo.access |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                       |  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      std::string debugName = str::format("depth ", m_desc.Id);
      imageInfo.debugName = debugName.c_str();
    } else {
      imageInfo.usage  |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    m_image = m_device->GetDXVKDevice()->createImage(imageInfo, memoryProperties);
  }

  GlideCommonTexture::~GlideCommonTexture() {
  }

  Rc<DxvkImageView> GlideCommonTexture::CreateView(
        UINT Lod,
        VkImageUsageFlags      UsageFlags,
        VkImageLayout          Layout) {
    DxvkImageViewKey viewInfo;
    viewInfo.format    = m_image->info().format;
    viewInfo.layout    = Layout;
    viewInfo.aspects   = lookupFormatInfo(viewInfo.format)->aspectMask;
    viewInfo.usage     = UsageFlags;
    viewInfo.viewType  = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.allowTypeMismatch = !(UsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT);
    viewInfo.mipIndex  = Lod;
    viewInfo.mipCount  = m_desc.MipMaps;
    viewInfo.layerIndex = 0u;
    viewInfo.layerCount = 1u;
    viewInfo.packedSwizzle = 0u;//DxvkImageViewKey::packSwizzle();

    // Remove the stencil aspect if we are trying to create a regular image
    // view of a depth stencil format
    if (!(UsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
      viewInfo.aspects &= ~VK_IMAGE_ASPECT_STENCIL_BIT;

    if (UsageFlags & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
      viewInfo.mipCount = 1;

    // Remove swizzle on depth views.
    if (UsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
      viewInfo.packedSwizzle = 0u;

    // Create the underlying image view object
    return GetImage()->createView(viewInfo);
  }

  const Rc<DxvkImageView>& GlideCommonTexture::GetView(UINT Lod) {
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (unlikely(!m_view)) {
      if (m_desc.Type == GR_BUFFER_AUXBUFFER || m_desc.Type == GR_BUFFER_DEPTHBUFFER) {
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      }
      if (m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_FRONTBUFFER) {
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }

      m_view = CreateView(Lod, usage, layout);
    }

    return m_view;
  }

}