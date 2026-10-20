#include "glide_texture.h"
#include "glide_device.h"
#include "glide_utils.h"

namespace dxvk {
  GlideCommonTexture::GlideCommonTexture(GlideDevice* pDevice, const GLIDE_COMMON_TEXTURE_DESC* pDesc)
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

    if (m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_FRONTBUFFER || m_desc.Type == GR_BUFFER_AUXBUFFER) {
      imageInfo.usage  |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                       |  VK_IMAGE_USAGE_SAMPLED_BIT;
      imageInfo.stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      imageInfo.access |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                       |  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      if (unlikely(m_device->GetDXVKDevice()->debugFlags().test(DxvkDebugFlag::Markers)))
        imageInfo.debugName = m_desc.Id.c_str();

      m_totalSize = m_desc.Width * m_desc.Height * 4;
    } else if (m_desc.Type == GR_BUFFER_DEPTHBUFFER) {
      imageInfo.usage  |= VK_IMAGE_ASPECT_STENCIL_BIT;
      if (unlikely(m_device->GetDXVKDevice()->debugFlags().test(DxvkDebugFlag::Markers)))
        imageInfo.debugName = m_desc.Id.c_str();

      m_totalSize = m_desc.Width * m_desc.Height * 2;
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
      if (m_desc.Type == GR_BUFFER_DEPTHBUFFER) {
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      }
      if (m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_FRONTBUFFER || m_desc.Type == GR_BUFFER_AUXBUFFER) {
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }

      m_view = CreateView(Lod, usage, layout);
    }

    return m_view;
  }


  void GlideCommonTexture::CreateBuffer(bool Initialize, uint32_t Size) {
    if (likely(m_buffer != nullptr))
      return;

    DxvkBufferCreateInfo info;
    info.size   = Size;
    info.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
                | VK_BUFFER_USAGE_TRANSFER_DST_BIT
                | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    info.stages = VK_PIPELINE_STAGE_TRANSFER_BIT
                | VK_PIPELINE_STAGE_HOST_BIT;
    info.access = VK_ACCESS_TRANSFER_READ_BIT
                | VK_ACCESS_TRANSFER_WRITE_BIT
                | VK_ACCESS_HOST_WRITE_BIT
                | VK_ACCESS_HOST_READ_BIT;

    if (m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_FRONTBUFFER) {
      info.usage  |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
      info.stages |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }

    VkMemoryPropertyFlags memType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                                  | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                                  | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

    m_buffer = m_device->GetDXVKDevice()->createBuffer(info, memType);

    if (Initialize) {
      if (m_data)
        std::memcpy(m_buffer->mapPtr(0), m_data.map(), m_totalSize);
      else
        std::memset(m_buffer->mapPtr(0), 0, m_totalSize);
    }
    m_data = {};
  }

  void GlideCommonTexture::ConsiderDownload() {
    if (unlikely(m_device->GetOptions()->hackNoLFB)) {
      return;
    }

    if (likely(!(m_dirty == TEXTURE_DIRTY_GPU && m_convertBuffer.size() > 0)))
      return;

    DEBUGLOG(str::format("Download buffer ", m_desc.Id, " from GPU"));

    size_t size = m_desc.Width * m_desc.Height * 4;
    if (m_buffer == nullptr)
      CreateBuffer(true, size);

    VkImageSubresourceRange subresources = m_image->getAvailableSubresources();
    VkImageSubresourceLayers layers = { subresources.aspectMask, subresources.baseMipLevel, subresources.baseArrayLayer, subresources.layerCount };
    VkExtent3D levelExtent = m_image->mipLevelExtent(subresources.baseMipLevel);
    DxvkBufferSlice slice = DxvkBufferSlice(m_buffer, 0, size);

    m_device->EmitFeedbackLoopBarriers(m_desc.Type == GR_BUFFER_FRONTBUFFER || m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_AUXBUFFER,
                                       m_desc.Type == GR_BUFFER_DEPTHBUFFER);
    m_device->EmitCs([
      cImageBufferSlice = slice,
      cImage            = m_image,
      cSubresources     = layers,
      cLevelExtent      = levelExtent,
      cPackedFormat     = m_desc.Format
    ] (DxvkContext* ctx) {
      ctx->copyImageToBuffer(cImageBufferSlice.buffer(),
        cImageBufferSlice.offset(), 0, 0, cPackedFormat,
        cImage, cSubresources, VkOffset3D { 0, 0, 0 },
        cLevelExtent);
    });

    m_device->ExecuteFlush();
    m_device->SynchronizeCsThread(m_device->GetCurrentSequenceNumber());
    m_device->GetDXVKDevice()->waitForResource(*m_buffer, DxvkAccess::Write);

    uint8_t* ptr = reinterpret_cast<uint8_t*>(m_buffer->mapPtr(0));
    if (ptr != nullptr) {
      UtilConvertRGBAToImage(UtilLFBWriteModeToIFormat(m_lockmode), ptr, m_convertBuffer.data(), m_desc.Width, m_desc.Height);

      if (unlikely(m_device->GetAPI() == GLIDEAPI::API_GLIDE_1X)) {
        std::vector<uint8_t> tempBuffer;
        constexpr FxU32 legacyWidth = 1024u;
        constexpr FxU32 legacyHeight = 1024u;
        FxU32 legacyStride = legacyWidth * m_blockSize;
        FxU32 legacySize = legacyWidth * legacyHeight * m_blockSize;
        const FxU32 w = std::min(m_desc.Width, legacyWidth);
        const FxU32 h = std::min(m_desc.Height, legacyHeight);
        size_t stride = m_desc.Width * m_blockSize;

        tempBuffer.resize(legacySize);

        for (FxU32 y = 0; y < h; ++y) {
          memcpy(tempBuffer.data() + y * stride, m_convertBuffer.data() + y * legacyStride, w * m_blockSize);
        }

        memcpy(m_convertBuffer.data(), tempBuffer.data(), legacySize);
      }
    } else {
      Logger::err("readback failed!");
    }
  }

  void GlideCommonTexture::ConsiderUpload() {
    if (unlikely(m_device->GetOptions()->hackNoLFB))
      return;

    if (likely(!(m_dirty == TEXTURE_DIRTY_CPU && m_convertBuffer.size() > 0)))
      return;

    DEBUGLOG(str::format("Uploading buffer ", m_desc.Id, " to GPU"));

    size_t size = m_desc.Width * m_desc.Height * 4;
    if (m_buffer == nullptr)
      CreateBuffer(true, size);

    VkImageSubresourceRange subresources = m_image->getAvailableSubresources();
    VkImageSubresourceLayers layers = { subresources.aspectMask, subresources.baseMipLevel, subresources.baseArrayLayer, subresources.layerCount };
    VkExtent3D levelExtent = m_image->mipLevelExtent(subresources.baseMipLevel);
    DxvkBufferSlice slice = DxvkBufferSlice(m_buffer, 0, size);

    if (unlikely(m_device->GetAPI() == GLIDEAPI::API_GLIDE_1X)) {
      std::vector<uint8_t> tempBuffer;
      constexpr FxU32 legacyWidth = 1024u;
      constexpr FxU32 legacyHeight = 1024u;
      FxU32 legacyStride = legacyWidth * m_blockSize;
      FxU32 legacySize = legacyWidth * legacyHeight * m_blockSize;
      const FxU32 w = std::min(m_desc.Width, legacyWidth);
      const FxU32 h = std::min(m_desc.Height, legacyHeight);
      size_t stride = m_desc.Width * m_blockSize;

      tempBuffer.resize(legacySize);
      memcpy(tempBuffer.data(), m_convertBuffer.data(), legacySize);
      // memset(tempBuffer.data(), 0xFF, legacySize / 4);

      for (FxU32 y = 0; y < h; ++y) {
        memcpy(m_convertBuffer.data() + y * stride, tempBuffer.data() + y * legacyStride, w * m_blockSize);
      }
    }

    FxU32 components = 4, dstPitch = 0;
    UtilConvertImageToRGBA(UtilLFBWriteModeToIFormat(m_lockmode), m_convertBuffer.data(), slice.mapPtr(0), components, dstPitch, m_desc.Width, m_desc.Height);

    m_device->EmitFeedbackLoopBarriers(m_desc.Type == GR_BUFFER_FRONTBUFFER || m_desc.Type == GR_BUFFER_BACKBUFFER || m_desc.Type == GR_BUFFER_AUXBUFFER,
                                       m_desc.Type == GR_BUFFER_DEPTHBUFFER);

    m_device->EmitCs([
      cSrcSlice       = slice,
      cDstImage       = m_image,
      cDstLayers      = layers,
      cDstLevelExtent = levelExtent,
      cColorFormat    = m_desc.Format
    ] (DxvkContext* ctx) mutable {
      ctx->copyBufferToImage(
        cDstImage,  cDstLayers,
        VkOffset3D { 0, 0, 0 }, cDstLevelExtent,
        cSrcSlice.buffer(), cSrcSlice.offset(),
        0, 0, cColorFormat);
    });

    m_device->ExecuteFlush();
    m_device->SynchronizeCsThread(m_device->GetCurrentSequenceNumber());
    m_device->GetDXVKDevice()->waitForResource(*m_buffer, DxvkAccess::Read);
  }

  void GlideCommonTexture::Clear(FxU32 color, VkRect2D region, FxBool partial) {
    if (m_convertBuffer.size() == 0)
      return;

    VkRect2D dimensions = VkRect2D{0, 0, m_desc.Width, m_desc.Height};
    UtilClearImage(UtilLFBWriteModeToIFormat(m_lockmode), m_device->GetColorFormat(), color, m_convertBuffer.data(), std::move(dimensions), region);
  }

  void* GlideCommonTexture::Lock(GrLock_t type, GrOriginLocation_t& origin, GrLfbWriteMode_t& mode) {
    // Sigh, some games do consecutive locks without unlocking...
    if (unlikely(m_image == nullptr))
      return nullptr;

    if (unlikely(origin == GR_ORIGIN_ANY))
      origin = GR_ORIGIN_UPPER_LEFT;

    if (mode == GR_LFBWRITEMODE_ANY) {
      // TODO: don't assume aux as depth.
      // For colorbuffers output should best match to our real buffer format but for original voodoo/rush it should be strictly GR_LFBWRITEMODE_565 according to docs.
      // For read-only locks hardware skipped format convertion and provided what is in the buffer as-is. But as we are converting everything to 32-bit RGBA we should convert properly back.
      // So most likely there are games with hardcoded assumption ANY = RGBA_565 no matter the card.
      // Also Diablo II wants rendering intros with GR_LFBWRITEMODE_8888 regardless of presented board!?
      if (unlikely(m_desc.Type == GR_BUFFER_DEPTHBUFFER)) {
        mode = GR_LFBWRITEMODE_ZA16;
      } else {
        mode = GR_LFBWRITEMODE_565;
      }
    }

    FxU32 blockSize = 0;
    UtilTextureFormatSize(UtilLFBWriteModeToIFormat(mode), &blockSize, nullptr, nullptr);
    m_blockSize = blockSize >> 3;
    if (unlikely(m_blockSize == 0))
      return nullptr;

    const FxU32 bufferSize = std::max(m_desc.Width * m_desc.Height * m_blockSize, 1024 * 1024 * 4u);
    if (unlikely(bufferSize > m_convertBuffer.size())) {
      m_convertBuffer.resize(bufferSize);
    }

    m_lockmode = mode;
    m_origin = origin;
    m_isLocked = true;

    if (likely(!m_device->GetOptions()->hackNoLFB))
      ConsiderDownload();

    // We set it there because there are games which re-request lock with bufferswap without unlocking
    if (type != GR_LFB_READ_ONLY)
      m_dirty = TEXTURE_DIRTY_CPU;

    return m_convertBuffer.data();
  }

  FxBool GlideCommonTexture::Unlock(GrLock_t type) {
    if (unlikely(m_image == nullptr || !m_isLocked))
      return FXFALSE;

    m_isLocked = false;

    return FXTRUE;
  }

}