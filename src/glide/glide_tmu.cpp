#include "glide_tmu.h"

#include "glide_device.h"
#include "glide_utils.h"

namespace dxvk {

  GlideTMU::GlideTMU(GlideDevice *parent, GrChipID_t id)
  : m_parent(parent)
  , m_id(id)
  , m_size(m_parent->GetBoardConfiguration()->tmuRam * 1024 * 1024)
  , m_buffer(std::make_unique<uint8_t[]>(m_size)) {
  }

  GlideTMU::~GlideTMU() {
  }

  FxBool GlideTMU::InsertTextures(TMUMetadata metadata, size_t start, size_t end) {
    if (unlikely(metadata.data == nullptr))
      return FXFALSE;

    GrIntFmt_t iformat = UtilTexFormatToInternalFormat(metadata.format);
    Logger::debug(str::format("TMU ", m_id, " texture insert request ", UtilInternalFormatToString(iformat), " ", metadata.smallLodLog2, "/", metadata.largeLodLog2, "(", metadata.aspectRatioLog2, ")"));

    std::vector<GlideMipMapOffset> mipmaps = {};
    metadata.size = UtilGetTextureSize(metadata.smallLodLog2, metadata.largeLodLog2, metadata.aspectRatioLog2, metadata.format, metadata.evenOdd, true, &mipmaps);
    for (const auto& mipmap : mipmaps) {
      TMUAddress subStartAddress = metadata.startAddress + mipmap.offset;
      TMUAddress subStopAddress = subStartAddress + mipmap.size;

      if (unlikely(subStopAddress >= GetMaximumAddress()))
        return FXFALSE;

      auto it = m_regions.lower_bound(subStartAddress);

      if (it != m_regions.begin()) {
        auto prev = std::prev(it);

        if (prev->second.stopAddress > subStartAddress)
          it = prev;
      }

      while (it != m_regions.end() && it->second.startAddress < subStopAddress) {
        it = m_regions.erase(it);
      }

      Logger::debug(str::format("TMU ", m_id, " texture ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " copying at address ", subStartAddress, "-", subStopAddress));
      memcpy(m_buffer.get() + subStartAddress, metadata.data, mipmap.size);
      Logger::debug(str::format("TMU ", m_id, " texture ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " has been uploaded at address ", subStartAddress, "-", subStopAddress));
    }

    return FXTRUE;
  }

  FxBool GlideTMU::InsertTextureLevel(TMUMetadata metadata, size_t start, size_t end) {
    if (unlikely(metadata.data == nullptr))
      return FXFALSE;

    GrIntFmt_t iformat = UtilTexFormatToInternalFormat(metadata.format);
    Logger::debug(str::format("TMU ", m_id, " texture insert request ", UtilInternalFormatToString(iformat), " ", metadata.smallLodLog2, "/", metadata.largeLodLog2, "(", metadata.aspectRatioLog2, ")"));

    std::vector<GlideMipMapOffset> mipmaps = {};
    metadata.size = UtilGetTextureSize(metadata.smallLodLog2, metadata.largeLodLog2, metadata.aspectRatioLog2, metadata.format, metadata.evenOdd, true, &mipmaps);

    if (unlikely(!metadata.size || !mipmaps.size()))
      return FXFALSE;

    auto& mipmap = mipmaps[mipmaps.size() - 1];
    TMUAddress subStartAddress = metadata.startAddress + mipmap.offset;
    TMUAddress subStopAddress = subStartAddress + mipmap.size;

    if (unlikely(subStopAddress >= GetMaximumAddress()))
      return FXFALSE;

    auto it = m_regions.lower_bound(subStartAddress);

    if (it != m_regions.begin()) {
      auto prev = std::prev(it);

      if (prev->second.stopAddress > subStartAddress)
        it = prev;
    }

    while (it != m_regions.end() && it->second.startAddress < subStopAddress) {
      it = m_regions.erase(it);
    }

    if (end != 0 && start < end) {
      end = std::min(end, mipmap.height);
      start = std::min(start, end);
      subStartAddress += start * mipmap.width * ((mipmap.blockSize * mipmap.blockWidth) >> 3);
      size_t updateSize = (end - start) * mipmap.width * ((mipmap.blockSize * mipmap.blockWidth) >> 3);
      subStopAddress = subStartAddress + updateSize;
      Logger::debug(str::format("TMU ", m_id, " mipmap ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " copying at address ", subStartAddress, "-", subStopAddress));
      memcpy(m_buffer.get() + subStartAddress, metadata.data, updateSize);
    } else {
      Logger::debug(str::format("TMU ", m_id, " mipmap ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " copying at address ", subStartAddress, "-", subStopAddress));
      memcpy(m_buffer.get() + subStartAddress, metadata.data, mipmap.size);
    }

    Logger::debug(str::format("TMU ", m_id, " mipmap ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " has been uploaded at address ", subStartAddress, "-", subStopAddress));

    return FXTRUE;
  }

  const GlideTMU::TMUTexture* GlideTMU::GetTexture(TMUMetadata& metadata) {
    if (unlikely(metadata.startAddress >= GetMaximumAddress()))
      return nullptr;

    auto it = m_regions.find(metadata.startAddress);
    if (it == m_regions.end())
      return nullptr;

    return &it->second;
  }

  void GlideTMU::SelectNCCTable(GrTexTable_t table) {
    if (unlikely(table != GR_TEXTABLE_NCC0 && table != GR_TEXTABLE_NCC1))
      return;

    m_currentNcc = table;
  }

  FxBool GlideTMU::SetTable(GrTexTable_t type, void *data, FxU32 start = 0, FxU32 stop = 0) {
    Logger::debug(str::format("TMU ", m_id, " table selection"));
    if (unlikely(data == nullptr))
      return FXFALSE;

    switch (type) {
      case GR_TEXTABLE_PALETTE: {
        start = std::clamp(start, 0U, VOODOO_PALETTE_TABLE_SIZE - 1);
        stop = std::clamp(stop, 0U, VOODOO_PALETTE_TABLE_SIZE - 1);
        if (start > stop)
          return FXFALSE;
        if (start == 0 && stop == 0)
          stop = VOODOO_PALETTE_TABLE_SIZE - 1;

        FxU32 length = stop - start;
        memcpy(m_palette.data() + start * sizeof(FxU32), data, length * sizeof(FxU32));
        m_hashes[0] = calculateHash(&m_palette);

        Logger::debug(str::format("TMU ", m_id, " palette applied"));
        return FXTRUE;
      }
      case GR_TEXTABLE_NCC0:
      case GR_TEXTABLE_NCC1: {
        start = std::clamp(start, 0U, VOODOO_NCC_TABLE_SIZE - 1);
        stop = std::clamp(stop, 0U, VOODOO_NCC_TABLE_SIZE - 1);

        if (start > stop)
          return FXFALSE;

        if (start == 0 && stop == 0)
          stop = VOODOO_NCC_TABLE_SIZE - 1;

        FxU32 length = stop - start;
        memcpy(m_ncc[type == GR_TEXTABLE_NCC0 ? 0 : 1].data() + start, data, length);
        m_hashes[GR_TEXTABLE_NCC0 ? 1 : 2] = calculateHash(&m_ncc[type == GR_TEXTABLE_NCC0 ? 0 : 1]);

        Logger::debug(str::format("TMU ", m_id, " NCC applied"));
        return FXTRUE;
      }
      case GR_TEXTABLE_PALETTE_6666_EXT:
      default:
        Logger::err(str::format("TMU ", m_id, " unsupported table type ", type));
        break;
    }

    return FXFALSE;
  }

  FxBool GlideTMU::UploadTextures(TMUMetadata metadata) {
    if (metadata.startAddress == GLIDE_NO_TEXTURE)
      return FXFALSE;

    GrIntFmt_t iformat = UtilTexFormatToInternalFormat(metadata.format);

    Logger::debug(str::format("TMU ", m_id, " upload to GPU request: ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(metadata.format)), " ", metadata.smallLodLog2, "/", metadata.largeLodLog2, "(", metadata.aspectRatioLog2, ")" ", address: ", metadata.startAddress));

    std::vector<GlideMipMapOffset> mipmaps;

    size_t size = UtilGetTextureSize(metadata.smallLodLog2, metadata.largeLodLog2, metadata.aspectRatioLog2, metadata.format, metadata.evenOdd, true, &mipmaps);
    size_t endAddress = metadata.startAddress + size;
    if (!mipmaps.size()) {
      Logger::debug(str::format("TMU ", m_id, " nothing to upload to GPU: ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(metadata.format)), " ", metadata.smallLodLog2, "/", metadata.largeLodLog2, "(", metadata.aspectRatioLog2, ")" ", address: ", metadata.startAddress));
      return FXFALSE;
    }

    auto it = m_regions.find(metadata.startAddress);
    if (it == m_regions.end()
           || it->second.stopAddress != endAddress
           || it->second.smallLodLog2 != metadata.smallLodLog2
           || it->second.largeLodLog2 != metadata.largeLodLog2
           || it->second.aspectRatioLog2 != metadata.aspectRatioLog2
           || it->second.evenOdd != metadata.evenOdd) {
      it = m_regions.lower_bound(metadata.startAddress);

      if (it != m_regions.begin()) {
        auto prev = std::prev(it);

        if (prev->second.stopAddress > metadata.startAddress)
          it = prev;
      }

      while (it != m_regions.end() && it->second.startAddress < endAddress) {
        it = m_regions.erase(it);
      }

      GlideTMU::TMUTexture t;
      t.startAddress = metadata.startAddress;
      t.stopAddress = endAddress;
      t.aspectRatioLog2 = metadata.aspectRatioLog2;
      t.format = metadata.format;
      t.evenOdd = metadata.evenOdd;
      t.smallLodLog2 = metadata.smallLodLog2;
      t.largeLodLog2 = metadata.largeLodLog2;
      t.size = size;
      t.height = mipmaps[0].height;
      t.width = mipmaps[0].width;
      t.upToDate = false;

      GLIDE_COMMON_TEXTURE_DESC desc;
      desc.Width              = mipmaps[0].width;
      desc.Height             = mipmaps[0].height;
      desc.MipMaps            = mipmaps.size();
      desc.Format             = VK_FORMAT_R8G8B8A8_UNORM;
      desc.Type               = GR_BUFFER_NONE;
      t.texture = new GlideCommonTexture(m_parent, &desc);

      auto [itn, success] = m_regions.emplace(metadata.startAddress, std::move(t));
      if (!success)
        return FXFALSE;

      it = itn;
    }

    GlideTMU::TMUTexture& tmuTexture = it->second;

    if (tmuTexture.texture && tmuTexture.upToDate) {
      FxBool skip = true;
      if (metadata.format == GR_TEXFMT_P_8 || metadata.format == GR_TEXFMT_P_8_6666 ||
        metadata.format == GR_TEXFMT_P_8_6666_EXT || metadata.format == GR_TEXFMT_AP_88) {
        skip = tmuTexture.hash == m_hashes[0];
      } else if (metadata.format == GR_TEXFMT_YIQ_422) {
        skip = tmuTexture.hash == m_hashes[1];
      } else if (metadata.format == GR_TEXFMT_AYIQ_8422) {
        skip = tmuTexture.hash == m_hashes[2];
      }

      if (likely(skip)) {
        Logger::debug(str::format("TMU ", m_id, " skip upload to GPU: ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(metadata.format)), " ", metadata.smallLodLog2, "/", metadata.largeLodLog2, "(", metadata.aspectRatioLog2, ")" ", address: ", metadata.startAddress));
        return FXTRUE;
      }
    }

    for (uint32_t idx = 0; idx < mipmaps.size(); idx++) {
      const GlideMipMapOffset& mipmap = mipmaps[idx];
      TMUAddress subStartAddress = metadata.startAddress + mipmap.offset;
      TMUAddress subStopAddress = subStartAddress + mipmap.size;
      if (unlikely(subStopAddress >= GetMaximumAddress()))
        return FXFALSE;

      Logger::debug(str::format("TMU ", m_id, " upload to GPU request mipmap: ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(metadata.format)), " ", mipmap.width, "x", mipmap.height, " address: ", subStartAddress, "-", subStopAddress));

      metadata.data = m_buffer.get() + metadata.startAddress + mipmap.offset;

      auto image = tmuTexture.texture->GetImage();
      if (unlikely(image == nullptr))
        return FXFALSE;

      VkOffset3D DestOffset{};
      auto subresources = image->getAvailableSubresources();
      VkExtent3D dstTexLevelExtent = image->mipLevelExtent(idx);
      auto formatInfo = image->formatInfo();
      VkOffset3D alignedDestOffset = {
        int32_t(alignDown(DestOffset.x, formatInfo->blockSize.width)),
        int32_t(alignDown(DestOffset.y, formatInfo->blockSize.height)),
        int32_t(alignDown(DestOffset.z, formatInfo->blockSize.depth))
      };
      VkExtent3D extentBlockCount = util::computeBlockCount(dstTexLevelExtent, formatInfo->blockSize);
      VkExtent3D alignedExtent = util::snapExtent3D(alignedDestOffset, extentBlockCount, dstTexLevelExtent);
      VkImageSubresourceLayers dstLayers = { subresources.aspectMask, idx, subresources.baseArrayLayer, subresources.layerCount };

      FxU32 components = 4, dstPitch = 0;
      size_t bufferSize = mipmap.width * mipmap.height * components;

      std::vector<uint8_t> pixelData;
      pixelData.resize(bufferSize);

      UtilConvertImageToRGBA(UtilTexFormatToInternalFormat(metadata.format), metadata.data, pixelData, components, dstPitch, mipmap.width, mipmap.height, &m_palette);
      VkDeviceSize pitch = align(dstPitch, components);

      auto slice = m_parent->GetStagingBuffer()->alloc(bufferSize);
      util::packImageData(
        slice.mapPtr(0), pixelData.data(), extentBlockCount, components,
        pitch, pitch * mipmap.height);

      m_parent->EmitCs([
        cSrcSlice       = slice,
        cDstImage       = image,
        cDstLayers      = dstLayers,
        cDstLevelExtent = alignedExtent,
        cOffset         = alignedDestOffset,
        cColorFormat    = VK_FORMAT_R8G8B8A8_UNORM
      ] (DxvkContext* ctx) {
        ctx->copyBufferToImage(
          cDstImage,  cDstLayers,
          cOffset, cDstLevelExtent,
          cSrcSlice.buffer(), cSrcSlice.offset(),
          0, 0, cColorFormat);
      });

      if (metadata.format == GR_TEXFMT_P_8 || metadata.format == GR_TEXFMT_P_8_6666 ||
          metadata.format == GR_TEXFMT_P_8_6666_EXT || metadata.format == GR_TEXFMT_AP_88) {
        tmuTexture.hash = calculateHash(&m_palette);
      } else if (metadata.format == GR_TEXFMT_YIQ_422) {
        tmuTexture.hash = calculateHash(&m_ncc[0]);
      } else if (metadata.format == GR_TEXFMT_AYIQ_8422) {
        tmuTexture.hash = calculateHash(&m_ncc[1]);
      }

      tmuTexture.upToDate = true;
      Logger::debug(str::format("TMU ", m_id, " mipmap ", UtilInternalFormatToString(iformat), " ", mipmap.width, "x", mipmap.height, " has been uploaded to GPU at address ", subStartAddress, "-", subStopAddress, ", hash: ", tmuTexture.hash));
    }

    return FXTRUE;
  }

}
