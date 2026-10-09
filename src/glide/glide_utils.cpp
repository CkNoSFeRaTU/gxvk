#include "glide_utils.h"

#include <algorithm>
#include <cassert>
#include <utility>

namespace dxvk {

  VkBlendFactor UtilDecodeBlendFactor(GrAlphaBlendFnc_t BlendFactor, bool alpha, bool source) {
    switch (BlendFactor) {
      default:
      case GR_BLEND_ZERO:
        return VK_BLEND_FACTOR_ZERO;
      case GR_BLEND_SRC_ALPHA:
        return VK_BLEND_FACTOR_SRC_ALPHA;
      case GR_BLEND_SRC_COLOR:
        return source ? VK_BLEND_FACTOR_DST_COLOR: VK_BLEND_FACTOR_SRC_COLOR;
      case GR_BLEND_DST_ALPHA:
        return VK_BLEND_FACTOR_DST_ALPHA;
      case GR_BLEND_ONE:
        return VK_BLEND_FACTOR_ONE;
      case GR_BLEND_ONE_MINUS_SRC_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      case GR_BLEND_ONE_MINUS_SRC_COLOR:
        return source ? VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR: VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
      case GR_BLEND_ONE_MINUS_DST_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
      case GR_BLEND_ALPHA_SATURATE: {
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn("Alpha saturate blend is currently unsupported!");
      }
    }

    return source ? VK_BLEND_FACTOR_ZERO : VK_BLEND_FACTOR_ONE;
  }

  glide3x::GrAspectRatio_t UtilAspectRatioToLOG2(glide2x::GrAspectRatio_t aspectRatio) {
    switch (aspectRatio) {
      case(glide2x::GR_ASPECT_1x1):
        return glide3x::GR_ASPECT_LOG2_1x1;
      case(glide2x::GR_ASPECT_1x2):
        return glide3x::GR_ASPECT_LOG2_1x2;
      case(glide2x::GR_ASPECT_1x4):
        return glide3x::GR_ASPECT_LOG2_1x4;
      case(glide2x::GR_ASPECT_1x8):
        return glide3x::GR_ASPECT_LOG2_1x8;
      case(glide2x::GR_ASPECT_2x1):
        return glide3x::GR_ASPECT_LOG2_2x1;
      case(glide2x::GR_ASPECT_4x1):
        return glide3x::GR_ASPECT_LOG2_4x1;
      case(glide2x::GR_ASPECT_8x1):
        return glide3x::GR_ASPECT_LOG2_8x1;
      default:
        break;
    }

    // shouldn't happen
    return glide3x::GR_ASPECT_LOG2_1x1;
  }

  glide2x::GrAspectRatio_t UtilAspectRatioToLegacy(glide3x::GrAspectRatio_t aspectRatio) {
    switch (aspectRatio) {
      case(glide3x::GR_ASPECT_LOG2_1x1):
        return glide2x::GR_ASPECT_1x1;
      case(glide3x::GR_ASPECT_LOG2_1x2):
        return glide2x::GR_ASPECT_1x2;
      case(glide3x::GR_ASPECT_LOG2_1x4):
        return glide2x::GR_ASPECT_1x4;
      case(glide3x::GR_ASPECT_LOG2_1x8):
        return glide2x::GR_ASPECT_1x8;
      case(glide3x::GR_ASPECT_LOG2_2x1):
        return glide2x::GR_ASPECT_2x1;
      case(glide3x::GR_ASPECT_LOG2_4x1):
        return glide2x::GR_ASPECT_4x1;
      case(glide3x::GR_ASPECT_LOG2_8x1):
        return glide2x::GR_ASPECT_8x1;
    }

    // shouldn't happen
    return glide2x::GR_ASPECT_1x1;
  }

  glide3x::GrLOD_t UtilLodToLOG2(glide2x::GrLOD_t lod) {
    switch (lod) {
      case(glide2x::GR_LOD_1):
        return glide3x::GR_LOD_LOG2_1;
      case(glide2x::GR_LOD_2):
        return glide3x::GR_LOD_LOG2_2;
      case(glide2x::GR_LOD_4):
        return glide3x::GR_LOD_LOG2_4;
      case(glide2x::GR_LOD_8):
        return glide3x::GR_LOD_LOG2_8;
      case(glide2x::GR_LOD_16):
        return glide3x::GR_LOD_LOG2_16;
      case(glide2x::GR_LOD_32):
        return glide3x::GR_LOD_LOG2_32;
      case(glide2x::GR_LOD_64):
        return glide3x::GR_LOD_LOG2_64;
      case(glide2x::GR_LOD_128):
        return glide3x::GR_LOD_LOG2_128;
      case(glide2x::GR_LOD_256):
        return glide3x::GR_LOD_LOG2_256;
      default:
        break;
    }

    // shouldn't happen
    return glide3x::GR_LOD_LOG2_1;
  }

  glide2x::GrLOD_t UtilLodToLegacy(glide3x::GrLOD_t lod) {
    switch (lod) {
      case(glide3x::GR_LOD_LOG2_1):
        return glide2x::GR_LOD_1;
      case(glide3x::GR_LOD_LOG2_2):
        return glide2x::GR_LOD_2;
      case(glide3x::GR_LOD_LOG2_4):
        return glide2x::GR_LOD_4;
      case(glide3x::GR_LOD_LOG2_8):
        return glide2x::GR_LOD_8;
      case(glide3x::GR_LOD_LOG2_16):
        return glide2x::GR_LOD_16;
      case(glide3x::GR_LOD_LOG2_32):
        return glide2x::GR_LOD_32;
      case(glide3x::GR_LOD_LOG2_64):
        return glide2x::GR_LOD_64;
      case(glide3x::GR_LOD_LOG2_128):
        return glide2x::GR_LOD_128;
      case(glide3x::GR_LOD_LOG2_256):
        return glide2x::GR_LOD_256;
      default:
        break;
    }

    // shouldn't happen
    return glide2x::GR_LOD_1;
  }

  void UtilTextureFormatSize(GrIntFmt_t format, FxU32* BlockSize, FxU32* BlockWidth, FxU32* BlockHeight) {
    if (BlockSize != nullptr)
      *BlockSize = 0;
    if (BlockWidth != nullptr)
      *BlockWidth = 1;
    if (BlockHeight != nullptr)
      *BlockHeight = 1;

    switch (format) {
      case GR_FMT_A_8:
      case GR_FMT_I_8:
      case GR_FMT_AI_44:
      case GR_FMT_P_8:
      case GR_FMT_RGB_332:
      case GR_FMT_YIQ_422:
      case GR_FMT_P_8_6666:
        if (BlockSize != nullptr)
          *BlockSize = 8;
        break;
      case GR_FMT_AYIQ_8422:
      case GR_FMT_ARGB_1555:
      case GR_FMT_ARGB_4444:
      case GR_FMT_ARGB_8332:
      case GR_FMT_AI_88:
      case GR_FMT_AP_88:
      case GR_FMT_RGB_565:
      case GR_FMT_ZA16:
        if (BlockSize != nullptr)
          *BlockSize = 16;
        break;
      case GR_FMT_RGB_888:
        if (BlockSize != nullptr)
          *BlockSize = 24;
        break;
      case GR_FMT_RGB_555_DEPTH:
      case GR_FMT_RGB_565_DEPTH:
      case GR_FMT_ARGB_1555_DEPTH:
      case GR_FMT_ARGB_8888:
        if (BlockSize != nullptr)
          *BlockSize = 32;
        break;
      case GR_FMT_YUYV_422:
      case GR_FMT_UYVY_422:
      case GR_FMT_AYUV_444:
        if (BlockWidth != nullptr)
          *BlockWidth = 2;
        if (BlockSize != nullptr)
          *BlockSize = 32;
        break;
      case GR_FMT_ARGB_CMP_FXT1:
      case GR_FMT_ARGB_CMP_DXT1:
        if (BlockWidth != nullptr)
          *BlockWidth = 4;
        if (BlockHeight != nullptr)
          *BlockHeight = 4;
        if (BlockSize != nullptr)
          *BlockSize = 64;
        break;
      case GR_FMT_ARGB_CMP_DXT2:
      case GR_FMT_ARGB_CMP_DXT3:
      case GR_FMT_ARGB_CMP_DXT4:
      case GR_FMT_ARGB_CMP_DXT5:
        if (BlockWidth != nullptr)
          *BlockWidth = 4;
        if (BlockHeight != nullptr)
          *BlockHeight = 4;
        if (BlockSize != nullptr)
          *BlockSize = 128;
        break;
      default:
        Logger::err(str::format("Unknown texture format: ", format));
        assert(0);
        break;
    }
  }

  size_t UtilGetTextureSize(glide3x::GrLOD_t smallLodLog2, glide3x::GrLOD_t largeLodLog2
    , glide3x::GrAspectRatio_t aspectRatioLog2, GrTextureFormat_t format
    , MipMapLevelMask_t evenOdd, FxBool round, std::vector<GlideMipMapOffset>* offsets) {
    size_t memSize = 0;

    if (smallLodLog2 > largeLodLog2)
      return memSize;

    if (evenOdd > GR_MIPMAPLEVELMASK_BOTH || evenOdd == GR_MIPMAPLEVELMASK_NONE)
      return memSize;

    glide3x::GrLOD_t thisSmallLodLog2 = smallLodLog2;
    glide3x::GrLOD_t thisLargeLodLog2 = largeLodLog2;
    while (thisLargeLodLog2 >= smallLodLog2) {
      FxU32 currentParity = (thisLargeLodLog2 & 1) ? GR_MIPMAPLEVELMASK_ODD : GR_MIPMAPLEVELMASK_EVEN;
      if ((evenOdd & currentParity) != 0) {
        FxU32 width, height, blockSize, blockWidth, blockHeight;
        UtilTextureDimensions(thisLargeLodLog2, aspectRatioLog2, width, height);
        UtilTextureFormatSize(UtilTexFormatToInternalFormat(format), &blockSize, &blockWidth, &blockHeight);
        FxU32 currentSize = 0;
        if (blockSize == 0 || height == 0 || width == 0)
          break;

        if (blockWidth > 1 || blockHeight > 1)
          currentSize = std::min(height * width, ((height + blockHeight - 1) / blockHeight) * ((width + blockWidth - 1) / blockWidth) * (blockSize / (blockWidth + blockHeight)));
        else
          currentSize = height * width * blockSize;

        if (offsets != nullptr) {
          GlideMipMapOffset offset;
          offset.smallLodLog2 = thisSmallLodLog2;
          offset.largeLodLog2 = thisLargeLodLog2;
          offset.size = currentSize >> 3;
          offset.offset = memSize >> 3;
          offset.width = width;
          offset.height = height;
          offset.blockSize = blockSize;
          offset.blockWidth = blockWidth;
          offset.blockHeight = blockHeight;
          offsets->push_back(std::move(offset));
        }

        memSize += currentSize;
      }
      thisSmallLodLog2 = static_cast<glide3x::GrLOD_t>(thisSmallLodLog2 - 1);
      thisLargeLodLog2 = static_cast<glide3x::GrLOD_t>(thisLargeLodLog2 - 1);
    }

    // bits to bytes
    memSize >>= 3;

    // round up to SST1 boundary
    if (round) {
      memSize += SST1_TEXTURE_ALIGN_MASK;
      memSize &= ~SST1_TEXTURE_ALIGN_MASK;
    }

    Logger::debug(str::format("format size calculation: ", UtilInternalFormatToString(UtilTexFormatToInternalFormat(format)), ", smallLodLog2: ", smallLodLog2, ", largeLodLog2: ", largeLodLog2, ", aspectRatioLog2: ", aspectRatioLog2, ", memSize: ", memSize));
    return memSize;
  }

  void UtilTextureDimensions(glide3x::GrLOD_t largeLodLog2, glide3x::GrAspectRatio_t aspectRatioLog2
    , FxU32& width, FxU32& height) {
    if (aspectRatioLog2 >= 0) {
      width = 1U << largeLodLog2;
      height = width >> aspectRatioLog2;
    } else {
      height = 1U << largeLodLog2;
      width = height >> -aspectRatioLog2;
    }
  }

  GrHwConfiguration UtilGetHWConfiguration(const GlideBoardConfiguration *config) {
    GrHwConfiguration result = { };

    result.num_sst = 1;
    result.SSTs->type = config->type;

    switch (config->type) {
      case(GR_SSTTYPE_AT3D):
        result.SSTs[0].sstBoard.AT3DConfig.rev = config->fbRev;
        break;
      case(GR_SSTTYPE_Voodoo):
        result.SSTs[0].sstBoard.VoodooConfig.fbiRev = config->fbRev;
        result.SSTs[0].sstBoard.VoodooConfig.fbRam = config->fbRam;
        result.SSTs[0].sstBoard.VoodooConfig.nTexelfx = config->tmuCount;
        result.SSTs[0].sstBoard.VoodooConfig.sliDetect = config->sli;
        for (FxI32 i = 0; i < std::min(config->tmuCount, GLIDE_NUM_TMU); i++) {
          result.SSTs[0].sstBoard.VoodooConfig.tmuConfig[i].tmuRev = config->tmuRev;
          result.SSTs[0].sstBoard.VoodooConfig.tmuConfig[i].tmuRam = config->tmuRam;
        }
        break;
      case(GR_SSTTYPE_SST96):
        result.SSTs[0].sstBoard.SST96Config.fbRam = config->fbRam;
        result.SSTs[0].sstBoard.SST96Config.nTexelfx = config->tmuCount;
        result.SSTs[0].sstBoard.SST96Config.tmuConfig.tmuRev = config->tmuRev;
        result.SSTs[0].sstBoard.SST96Config.tmuConfig.tmuRam = config->tmuRam;
        break;
      default:
        result.SSTs[0].sstBoard.Voodoo2Config.fbiRev = config->fbRev;
        result.SSTs[0].sstBoard.Voodoo2Config.fbRam = config->fbRam;
        result.SSTs[0].sstBoard.Voodoo2Config.nTexelfx = config->tmuCount;
        result.SSTs[0].sstBoard.VoodooConfig.sliDetect = config->sli;
        for (FxI32 i = 0; i < std::min(config->tmuCount, GLIDE_NUM_TMU); i++) {
          result.SSTs[0].sstBoard.Voodoo2Config.tmuConfig[i].tmuRev = config->tmuRev;
          result.SSTs[0].sstBoard.Voodoo2Config.tmuConfig[i].tmuRam = config->tmuRam;
        }
        break;
    }

    return result;
  }

  GlideBoardConfiguration UtilGetBoardConfiguration(GLIDEAPI api, const GlideOptions *options) {
    switch (options->card) {
      case GlideCard::CARD_VOODOO:
        return GlideBoards[0];
      case GlideCard::CARD_RUSH:
        return GlideBoards[1];
      case GlideCard::CARD_VOODOO2:
        return GlideBoards[2];
      case GlideCard::CARD_BANSHEE:
        return GlideBoards[3];
      case GlideCard::CARD_VOODOO3:
        return GlideBoards[4];
      case GlideCard::CARD_VOODOO4:
        return GlideBoards[5];
      case GlideCard::CARD_VOODOO5:
        return GlideBoards[6];
      case GlideCard::CARD_AUTO:
      default:
        if (api == GLIDEAPI::API_GLIDE_1X)
          return GlideBoards[0];
        else if (api == GLIDEAPI::API_GLIDE_2X)
          return GlideBoards[2];
        else
          return GlideBoards[5];
    }

    return GlideBoards[2];
  }

  VkSamplerAddressMode UtilGlideClampToVKSamplerAddressMode(GrTextureClampMode_t mode) {
    switch (mode) {
      case GR_TEXTURECLAMP_WRAP:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
      case GR_TEXTURECLAMP_CLAMP:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
      case GR_TEXTURECLAMP_MIRROR_EXT:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    }

    return VK_SAMPLER_ADDRESS_MODE_REPEAT;
  }

  VkCullModeFlags UtilCullModeVKCullModeFlags(GrCullMode_t mode) {
    // Note: cullmode choice depends on CW/CCW and need to align with out viewport -height + CW/CCW swap based on GrOriginLocation_t
    switch (mode) {
      case GR_CULL_DISABLE:
        return VK_CULL_MODE_NONE;
      case GR_CULL_NEGATIVE:
        return VK_CULL_MODE_BACK_BIT;
      case GR_CULL_POSITIVE:
        return VK_CULL_MODE_FRONT_BIT;
      return VK_CULL_MODE_BACK_BIT;
    }

    return VK_CULL_MODE_NONE;
  }

  VkCompareOp UtilCompareModeToVKCompareMode(GrCmpFnc_t fnc) {
    switch (fnc) {
      case GR_CMP_NEVER:
        return VK_COMPARE_OP_NEVER;
      case GR_CMP_LESS:
        return VK_COMPARE_OP_LESS;
      case GR_CMP_EQUAL:
        return VK_COMPARE_OP_EQUAL;
      case GR_CMP_LEQUAL:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
      case GR_CMP_GREATER:
        return VK_COMPARE_OP_GREATER;
      case GR_CMP_NOTEQUAL:
        return VK_COMPARE_OP_NOT_EQUAL;
      case GR_CMP_GEQUAL:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
      case GR_CMP_ALWAYS:
        return VK_COMPARE_OP_ALWAYS;
    }

    return VK_COMPARE_OP_NEVER;
  }

  FxBool UtilRefreshRateToNumber(GrScreenRefresh_t refreshRate, FxU32& number, FxBool extended) {
    if (extended) {
      number = refreshRate >> 16;
    } else if (refreshRate >= GR_REFRESH_MIN && refreshRate <= GR_REFRESH_MAX) {
      number = GlideStaticRefreshRates[refreshRate].number;
    } else {
      return FXFALSE;
    }

    return FXTRUE;
  }

  FxBool UtilNumberToRefreshRate(FxU32 number, GrScreenRefresh_t& refreshRate, FxBool extended) {
    if (extended) {
      // non-3Dfx resolution extension, NFS3 mod uses this
      refreshRate = static_cast<GrScreenRefresh_t>(refreshRate << 16);
      return FXTRUE;
    } else {
      for (const auto& curRefreshRate: GlideStaticRefreshRates) {
        if (curRefreshRate.number == number) {
          refreshRate = curRefreshRate.refreshRate;
          return FXTRUE;
        }
      }
    }

    return FXFALSE;
  }

  FxBool UtilResolutionToDimensions(GrScreenResolution_t resolution, FxU32& width, FxU32& height, FxBool extended) {
    if (extended) {
      // non-3Dfx resolution extension, NFS3 mod uses this
      width = resolution & 0xFFFF;
      height = resolution >> 16;
    } else if (resolution >= GR_RESOLUTION_MIN && resolution <= GR_RESOLUTION_MAX) {
      width = GlideStaticResolutions[resolution].width;
      height = GlideStaticResolutions[resolution].height;
    } else {
      return FXFALSE;
    }

    return FXTRUE;
  }

  FxBool UtilDimensionsToResolution(FxU32 width, FxU32 height, GrScreenResolution_t& resolution, FxBool extended) {
    if (extended) {
      // non-3Dfx resolution extension, NFS3 mod uses this
      resolution = static_cast<GrScreenResolution_t>((height << 16) | (width & 0xFFFF));
      return FXTRUE;
    } else {
      for (const auto& curResolution: GlideStaticResolutions) {
        if (curResolution.width == width && curResolution.height) {
          resolution = curResolution.resolution;
          return FXTRUE;
        }
      }
    }

    return FXFALSE;
  }

  std::string UtilInternalFormatToString(GrIntFmt_t format) {
    switch (format) {
    case GR_FMT_RGB_332:
      return "GR_FMT_RGB_332";
    case GR_FMT_RGB_555:
      return "GR_FMT_RGB_555";
    case GR_FMT_RGB_565:
      return "GR_FMT_RGB_565";
    case GR_FMT_RGB_888:
      return "GR_FMT_RGB_888";
    case GR_FMT_ARGB_1555:
      return "GR_FMT_ARGB_1555";
    case GR_FMT_ARGB_4444:
      return "GR_FMT_ARGB_4444";
    case GR_FMT_ARGB_8888:
      return "GR_FMT_ARGB_8888";
    case GR_FMT_ARGB_8332:
      return "GR_FMT_ARGB_8332";
    case GR_FMT_YIQ_422:
      return "GR_FMT_YIQ_442";
    case GR_FMT_AYIQ_8422:
      return "GR_FMT_AYIQ_8442";
    case GR_FMT_A_8:
      return "GR_FMT_A_8";
    case GR_FMT_I_8:
      return "GR_FMT_I_8";
    case GR_FMT_P_8:
      return "GR_FMT_P_8";
    case GR_FMT_AI_44:
      return "GR_FMT_AI_44";
    case GR_FMT_AP_88:
      return "GR_FMT_AP_88";
    case GR_FMT_ZA16:
      return "GR_FMT_ZA16";
    case GR_FMT_RLE16:
      return "GR_FMT_RLE16";
    case GR_FMT_ARGB_CMP_FXT1:
      return "GR_FMT_ARGB_CMP_FXT1";
    case GR_FMT_YUYV_422:
      return "GR_FMT_YUYV_422";
    case GR_FMT_UYVY_422:
      return "GR_FMT_UYVY_422";
    case GR_FMT_AYUV_444:
      return "GR_FMT_AYUV_444";
    case GR_FMT_ARGB_CMP_DXT1:
      return "GR_FMT_ARGB_CMP_DXT1";
    case GR_FMT_ARGB_CMP_DXT2:
      return "GR_FMT_ARGB_CMP_DXT2";
    case GR_FMT_ARGB_CMP_DXT3:
      return "GR_FMT_ARGB_CMP_DXT3";
    case GR_FMT_ARGB_CMP_DXT4:
      return "GR_FMT_ARGB_CMP_DXT4";
    case GR_FMT_ARGB_CMP_DXT5:
      return "GR_FMT_ARGB_CMP_DXT5";
    default:
      return "Unknown";
    }
  }

  GrIntFmt_t UtilTexFormatToInternalFormat(GrTextureFormat_t format) {
    switch (format) {
      case(GR_TEXFMT_RGB_332):
        return GR_FMT_RGB_332;
      case(GR_TEXFMT_YIQ_422):
        return GR_FMT_YIQ_422;
      case(GR_TEXFMT_ALPHA_8):
        return GR_FMT_A_8;
      case(GR_TEXFMT_INTENSITY_8):
        return GR_FMT_I_8;
      case(GR_TEXFMT_ALPHA_INTENSITY_44):
        return GR_FMT_AI_44;
      case(GR_TEXFMT_P_8):
        return GR_FMT_P_8;
      case(GR_TEXFMT_ARGB_8332):
        return GR_FMT_ARGB_8332;
      case(GR_TEXFMT_AYIQ_8422):
        return GR_FMT_AYIQ_8422;
      case(GR_TEXFMT_RGB_565):
        return GR_FMT_RGB_565;
      case(GR_TEXFMT_ARGB_1555):
        return GR_FMT_ARGB_1555;
      case(GR_TEXFMT_ARGB_4444):
        return GR_FMT_ARGB_4444;
      case(GR_TEXFMT_ALPHA_INTENSITY_88):
        return GR_FMT_AI_88;
      case(GR_TEXFMT_AP_88):
        return GR_FMT_AP_88;
      case(GR_TEXFMT_ARGB_CMP_FXT1):
        return GR_FMT_ARGB_CMP_FXT1;
      case(GR_TEXFMT_ARGB_8888):
        return GR_FMT_ARGB_8888;
      case(GR_TEXFMT_YUYV_422):
        return GR_FMT_YUYV_422;
      case(GR_TEXFMT_UYVY_422):
        return GR_FMT_UYVY_422;
      case(GR_TEXFMT_AYUV_444):
        return GR_FMT_AYUV_444;
      case(GR_TEXFMT_ARGB_CMP_DXT1):
        return GR_FMT_ARGB_CMP_DXT1;
      case(GR_TEXFMT_ARGB_CMP_DXT2):
        return GR_FMT_ARGB_CMP_DXT2;
      case(GR_TEXFMT_ARGB_CMP_DXT3):
        return GR_FMT_ARGB_CMP_DXT3;
      case(GR_TEXFMT_ARGB_CMP_DXT4):
        return GR_FMT_ARGB_CMP_DXT4;
      case(GR_TEXFMT_ARGB_CMP_DXT5):
        return GR_FMT_ARGB_CMP_DXT5;
      case(GR_TEXFMT_RGB_888):
        return GR_FMT_RGB_888;
      case(GR_TEXFMT_P_8_6666):
        return GR_FMT_P_8_6666;
      default:
        Logger::err(str::format("Unknown Glide texture format: ", format));
        assert(0);
        break;
    }

    return GR_FMT_UNKNOWN;
  }

  GrTextureFormat_t UtilInternalFormatToTexFormat(GrIntFmt_t format) {
    switch (format) {
      case(GR_FMT_RGB_332):
        return GR_TEXFMT_RGB_332;
      case(GR_FMT_YIQ_422):
        return GR_TEXFMT_YIQ_422;
      case(GR_FMT_A_8):
        return GR_TEXFMT_ALPHA_8;
      case(GR_FMT_I_8):
        return GR_TEXFMT_INTENSITY_8;
      case(GR_FMT_AI_44):
        return GR_TEXFMT_ALPHA_INTENSITY_44;
      case(GR_FMT_P_8):
        return GR_TEXFMT_P_8;
      case(GR_FMT_ARGB_8332):
        return GR_TEXFMT_ARGB_8332;
      case(GR_FMT_AYIQ_8422):
        return GR_TEXFMT_AYIQ_8422;
      case(GR_FMT_RGB_565):
        return GR_TEXFMT_RGB_565;
      case(GR_FMT_ARGB_1555):
        return GR_TEXFMT_ARGB_1555;
      case(GR_FMT_ARGB_4444):
        return GR_TEXFMT_ARGB_4444;
      case(GR_FMT_AI_88):
        return GR_TEXFMT_ALPHA_INTENSITY_88;
      case(GR_FMT_AP_88):
        return GR_TEXFMT_AP_88;
      case(GR_FMT_ARGB_CMP_FXT1):
        return GR_TEXFMT_ARGB_CMP_FXT1;
      case(GR_FMT_ARGB_8888):
        return GR_TEXFMT_ARGB_8888;
      case(GR_FMT_YUYV_422):
        return GR_TEXFMT_YUYV_422;
      case(GR_FMT_UYVY_422):
        return GR_TEXFMT_UYVY_422;
      case(GR_FMT_AYUV_444):
        return GR_TEXFMT_AYUV_444;
      case(GR_FMT_ARGB_CMP_DXT1):
        return GR_TEXFMT_ARGB_CMP_DXT1;
      case(GR_FMT_ARGB_CMP_DXT2):
        return GR_TEXFMT_ARGB_CMP_DXT2;
      case(GR_FMT_ARGB_CMP_DXT3):
        return GR_TEXFMT_ARGB_CMP_DXT3;
      case(GR_FMT_ARGB_CMP_DXT4):
        return GR_TEXFMT_ARGB_CMP_DXT4;
      case(GR_FMT_ARGB_CMP_DXT5):
        return GR_TEXFMT_ARGB_CMP_DXT5;
      case(GR_FMT_RGB_888):
        return GR_TEXFMT_RGB_888;
      default:
        Logger::err(str::format("Unknown internal texture format: ", format));
        assert(0);
        break;
    }

    return GR_TEXFMT_RGB_332;
  }

  GrIntFmt_t UtilLFBWriteModeToIFormat(GrLfbWriteMode_t writeMode) {
    switch (writeMode) {
      case GR_LFBWRITEMODE_555:
        return GR_FMT_RGB_555;
      case GR_LFBWRITEMODE_ANY:
      case GR_LFBWRITEMODE_565:
        return GR_FMT_RGB_565;
      case GR_LFBWRITEMODE_1555:
        return GR_FMT_ARGB_1555;
      case GR_LFBWRITEMODE_888:
        return GR_FMT_RGB_888;
      case GR_LFBWRITEMODE_8888:
        return GR_FMT_ARGB_8888;
      case GR_LFBWRITEMODE_555_DEPTH:
        return GR_FMT_RGB_555_DEPTH;
      case GR_LFBWRITEMODE_565_DEPTH:
        return GR_FMT_RGB_565_DEPTH;
      case GR_LFBWRITEMODE_1555_DEPTH:
        return GR_FMT_ARGB_1555_DEPTH;
      case GR_LFBWRITEMODE_ZA16:
        return GR_FMT_ZA16;
      default:
        Logger::err(str::format("Unknown Glide writemode format: ", writeMode));
        assert(0);
        break;
    }

    return GR_FMT_UNKNOWN;
  }

  static inline void YUV2RGB(int Y, int U, int V, uint8_t& r, uint8_t& g, uint8_t& b, bool use601)
  {
      int C = Y - 16;
      int D = U - 128;
      int E = V - 128;

      int Rt = (298 * C + (use601 ? 409 : 459) * E + 128) >> 8;
      int Gt = (298 * C - (use601 ? 100 : 55) * D - (use601 ? 208 : 136) * E + 128) >> 8;
      int Bt = (298 * C + (use601 ? 516 : 541) * D + 128) >> 8;

      r = std::clamp(Rt, 0, 255);
      g = std::clamp(Gt, 0, 255);
      b = std::clamp(Bt, 0, 255);
  }

  FxBool UtilConvertImageToRGBA(GrIntFmt_t format, void *srcPtr, std::vector<uint8_t>& dstVector, FxU32& dstComponents, FxU32& dstPitch, FxU32 width, FxU32 height, const std::array<FxU32, VOODOO_PALETTE_TABLE_SIZE> *palette) {
    FxU32 blockSize = 0;
    UtilTextureFormatSize(format, &blockSize, nullptr, nullptr);
    size_t srcPitch = width * (blockSize >> 3);
    switch (format) {
      case GR_FMT_RGB_332:
      case GR_FMT_RGB_555:
      case GR_FMT_RGB_555_DEPTH:
      case GR_FMT_RGB_565:
      case GR_FMT_RGB_565_DEPTH:
      case GR_FMT_RGB_888:
      case GR_FMT_P_8:
      case GR_FMT_UYVY_422:
      case GR_FMT_YUYV_422:
        if (dstComponents != 4)
          dstComponents = 3;
        break;
      case GR_FMT_ARGB_1555:
      case GR_FMT_ARGB_1555_DEPTH:
      case GR_FMT_ARGB_4444:
      case GR_FMT_ARGB_8332:
      case GR_FMT_ARGB_8888:
      case GR_FMT_A_8:
      case GR_FMT_I_8:
      case GR_FMT_AI_44:
      case GR_FMT_AI_88:
      case GR_FMT_AP_88:
        dstComponents = 4;
        break;
      case GR_FMT_ARGB_CMP_DXT1:
        //srcPitch = (width + 3 / 4) * (64 / 8);
      case GR_FMT_ARGB_CMP_DXT2:
      case GR_FMT_ARGB_CMP_DXT3:
      case GR_FMT_ARGB_CMP_DXT4:
      case GR_FMT_ARGB_CMP_DXT5:
        //srcPitch = (width + 3 / 4) * (128 / 8);
      default:
        Logger::err(str::format("unsupported input format: ", UtilInternalFormatToString(format), "(", format, ")"));
        return FXFALSE;
    }

    if (unlikely((format == GR_FMT_P_8 || format == GR_FMT_AP_88) && palette == nullptr)) {
      Logger::err(str::format("paletted format: ", UtilInternalFormatToString(format), " without palette"));
      return FXFALSE;
    }

    dstPitch = width * dstComponents;
    size_t imageSize = dstPitch * height;
    dstVector.resize(imageSize);

    const bool use601 = true;
    const uint8_t *src = (const uint8_t*)srcPtr;
    uint8_t *dst = dstVector.data();
    for (uint32_t y = 0; y < height; ++y) {
      switch (format) {
        case GR_FMT_RGB_332:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = src[x];
            dst[dstComponents * x + 0] = ((pixel >> 5)       ) * 0xff / 0x07;
            dst[dstComponents * x + 1] = ((pixel >> 2) & 0x07) * 0xff / 0x07;
            dst[dstComponents * x + 2] = ( pixel       & 0x03) * 0xff / 0x03;
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_ARGB_8332:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            dst[dstComponents * x + 0] = ((pixel >> 5) & 0x07) * 0xff / 0x07;
            dst[dstComponents * x + 1] = ((pixel >> 2) & 0x07) * 0xff / 0x07;
            dst[dstComponents * x + 2] = ( pixel       & 0x03) * 0xff / 0x03;
            dst[dstComponents * x + 3] = ((pixel >> 8) & 0xff);
          }
          break;
        case GR_FMT_RGB_555:
        case GR_FMT_RGB_555_DEPTH:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            dst[dstComponents * x + 0] = (( pixel >> 11        ) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 1] = (((pixel >>  5) & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 2] = (( pixel        & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_RGB_565:
        case GR_FMT_RGB_565_DEPTH:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            dst[dstComponents * x + 0] = (( pixel >> 11        ) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 1] = (((pixel >>  5) & 0x3f) * (2*0xff) + 0x3f) / (2*0x3f);
            dst[dstComponents * x + 2] = (( pixel        & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_RGB_888:
          for (uint32_t x = 0; x < width; ++x) {
            dst[dstComponents * x + 0] = src[dstComponents * x + 0];
            dst[dstComponents * x + 1] = src[dstComponents * x + 1];
            dst[dstComponents * x + 2] = src[dstComponents * x + 2];
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_ARGB_1555:
        case GR_FMT_ARGB_1555_DEPTH:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            dst[dstComponents * x + 0] = (((pixel >> 10) & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 1] = (((pixel >>  5) & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 2] = (( pixel        & 0x1f) * (2*0xff) + 0x1f) / (2*0x1f);
            dst[dstComponents * x + 3] = (((pixel >> 15)       ) ? 255 : 0);
          }
          break;
        case GR_FMT_ARGB_4444:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            dst[dstComponents * x + 0] = ((pixel >> 12) & 0x0f) * 0x11;
            dst[dstComponents * x + 1] = ((pixel >> 8)  & 0x0f) * 0x11;
            dst[dstComponents * x + 2] = ((pixel >> 4 ) & 0x0f) * 0x11;
            dst[dstComponents * x + 3] = ( pixel        & 0x0f) * 0x11;
          }
          break;
        case GR_FMT_ARGB_8888:
          for (uint32_t x = 0; x < width; ++x) {
            dst[dstComponents * x + 2] = src[dstComponents * x + 0];
            dst[dstComponents * x + 1] = src[dstComponents * x + 1];
            dst[dstComponents * x + 0] = src[dstComponents * x + 2];
            dst[dstComponents * x + 3] = src[dstComponents * x + 3];
          }
          break;
        case GR_FMT_A_8:
          for (uint32_t x = 0; x < width; ++x) {
            dst[dstComponents * x + 0] = 0xff;
            dst[dstComponents * x + 1] = 0xff;
            dst[dstComponents * x + 2] = 0xff;
            dst[dstComponents * x + 3] = src[x];
          }
          break;
        case GR_FMT_I_8:
          for (uint32_t x = 0; x < width; ++x) {
            dst[dstComponents * x + 0] = src[x];
            dst[dstComponents * x + 1] = src[x];
            dst[dstComponents * x + 2] = src[x];
            dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_AI_44:
          for (uint32_t x = 0; x < width; ++x) {
            const uint8_t intensity = (src[x] & 0x0f) * 0x11;
            dst[dstComponents * x + 0] = intensity;
            dst[dstComponents * x + 1] = intensity;
            dst[dstComponents * x + 2] = intensity;
            dst[dstComponents * x + 3] = (src[x] >> 4 & 0x0f) * 0x11;
          }
          break;
        case GR_FMT_AI_88:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = ((const uint16_t *)src)[x];
            const uint8_t intensity = pixel & 0xff;
            dst[dstComponents * x + 0] = intensity;
            dst[dstComponents * x + 1] = intensity;
            dst[dstComponents * x + 2] = intensity;
            dst[dstComponents * x + 3] = (pixel >> 8) & 0xff;
          }
          break;
        case GR_FMT_P_8:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = palette->at(src[x]);
            dst[dstComponents * x + 0] = (pixel >> 16) & 0xFF;
            dst[dstComponents * x + 1] = (pixel >> 8 ) & 0xFF;
            dst[dstComponents * x + 2] = (pixel      ) & 0xFF;
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;
          }
          break;
        case GR_FMT_AP_88:
          for (uint32_t x = 0; x < width; ++x) {
            const uint32_t pixel = palette->at(src[2*x + 0]);
            dst[dstComponents * x + 0] = (pixel >> 16) & 0xFF;
            dst[dstComponents * x + 1] = (pixel >> 8) & 0xFF;
            dst[dstComponents * x + 2] = (pixel >> 0) & 0xFF;
            dst[dstComponents * x + 3] = src[2*x + 1];
          }
          break;
        case GR_FMT_UYVY_422:
          for (uint32_t x = 0; x < width; x += 2) {
            uint8_t r, g, b;
            YUV2RGB(src[x * 2 + 1], src[x * 2], src[x * 2 + 2], r, g, b, use601);
            dst[dstComponents * x + 0] = r;
            dst[dstComponents * x + 1] = g;
            dst[dstComponents * x + 2] = b;
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;

            YUV2RGB(src[x * 2 + 3], src[x * 2], src[x * 2 + 2], r, g, b, use601);
            dst[dstComponents * x + 3 + (dstComponents == 4 ? 1 : 0)] = r;
            dst[dstComponents * x + 4 + (dstComponents == 4 ? 1 : 0)] = g;
            dst[dstComponents * x + 5 + (dstComponents == 4 ? 1 : 0)] = b;
            if (dstComponents == 4)
              dst[dstComponents * x + 7] = 0xFF;
          }
          break;
        case GR_FMT_YUYV_422:
          for (uint32_t x = 0; x < width; x += 2) {
            uint8_t r, g, b;
            YUV2RGB(src[x * 2 + 0], src[x * 2 + 1], src[x * 2 + 3], r, g, b, use601);
            dst[dstComponents * x + 0] = r;
            dst[dstComponents * x + 1] = g;
            dst[dstComponents * x + 2] = b;
            if (dstComponents == 4)
              dst[dstComponents * x + 3] = 0xFF;

            YUV2RGB(src[x * 2 + 2], src[x * 2 + 1], src[x * 2 + 3], r, g, b, use601);
            dst[dstComponents * x + 3 + (dstComponents == 4 ? 1 : 0)] = r;
            dst[dstComponents * x + 4 + (dstComponents == 4 ? 1 : 0)] = g;
            dst[dstComponents * x + 5 + (dstComponents == 4 ? 1 : 0)] = b;
            if (dstComponents == 4)
              dst[dstComponents * x + 7] = 0xFF;
          }
          break;
        default:
          return FXFALSE;
      }

      src+= srcPitch;
      dst+= dstPitch;
    }

    Logger::debug(str::format("successfully converted format: ", UtilInternalFormatToString(format), " to RGBA"));
    return FXTRUE;
  }

  VkPrimitiveTopology UtilVertexModeToTopology(glide3x::GrDrawVertexArrayMode_t mode) {
    switch (mode) {
      case glide3x::GR_POINTS:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
      case glide3x::GR_LINE_STRIP:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
      case glide3x::GR_LINES:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
      case glide3x::GR_TRIANGLE_FAN_CONTINUE: {
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn("Triangle fan continuation is currently unsupported!");
      }
        //fall-through
      case glide3x::GR_POLYGON:
      case glide3x::GR_TRIANGLE_FAN:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
      case glide3x::GR_TRIANGLE_STRIP_CONTINUE: {
        static bool sWarnShown = false;
        if (!std::exchange(sWarnShown, true))
          Logger::warn("Triangle strip continuation is currently unsupported!");
      }
        //fall-through
      case glide3x::GR_TRIANGLE_STRIP:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
      case glide3x::GR_TRIANGLES:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
  }

}
