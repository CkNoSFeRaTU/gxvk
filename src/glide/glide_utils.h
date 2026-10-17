#pragma once

#include "glide_include.h"
#include "glide_options.h"

namespace dxvk {

  struct GlideMipMapOffset {
    glide3x::GrLOD_t smallLodLog2 = glide3x::GR_LOD_LOG2_1;
    glide3x::GrLOD_t largeLodLog2 = glide3x::GR_LOD_LOG2_1;
    FxU32 offset = 0;
    FxU32 size = 0;
    FxU32 width = 0;
    FxU32 height = 0;
    FxU32 blockSize = 0;
    FxU32 blockWidth = 0;
    FxU32 blockHeight = 0;
  };

  VkBlendFactor UtilDecodeBlendFactor(GrAlphaBlendFnc_t BlendFactor, bool alpha, bool source);
  glide3x::GrAspectRatio_t UtilAspectRatioToLOG2(glide2x::GrAspectRatio_t aspectRatio);
  glide2x::GrAspectRatio_t UtilAspectRatioToLegacy(glide3x::GrAspectRatio_t aspectRatio);
  glide3x::GrLOD_t UtilLodToLOG2(glide2x::GrLOD_t lod);
  glide2x::GrLOD_t UtilLodToLegacy(glide3x::GrLOD_t lod);

  FxBool UtilRefreshRateToNumber(GrScreenRefresh_t refreshRate, FxU32& number, FxBool extended);
  FxBool UtilNumberToRefreshRate(FxU32 number, GrScreenRefresh_t& refreshRate, FxBool extended);
  FxBool UtilResolutionToDimensions(GrScreenResolution_t resolution, FxU32& width, FxU32& height, FxBool extended);
  FxBool UtilDimensionsToResolution(FxU32 width, FxU32 height, GrScreenResolution_t& resolution, FxBool extended);

  size_t UtilGetTextureSize(glide3x::GrLOD_t smallLodLog2, glide3x::GrLOD_t largeLodLog2
    , glide3x::GrAspectRatio_t aspectRatioLog2, GrTextureFormat_t format
    , MipMapLevelMask_t evenOdd = GR_MIPMAPLEVELMASK_BOTH, FxBool round = FXTRUE, std::vector<GlideMipMapOffset>* offsets = nullptr);
  void UtilTextureDimensions(glide3x::GrLOD_t largeLodLog2
    , glide3x::GrAspectRatio_t aspectRatioLog2, FxU32& width, FxU32& height);
  void UtilTextureFormatSize(GrIntFmt_t format, FxU32* BlockSize, FxU32* BlockWidth = nullptr, FxU32* BlockHeight = nullptr);
  GrHwConfiguration UtilGetHWConfiguration(const GlideBoardConfiguration *config);
  GlideBoardConfiguration UtilGetBoardConfiguration(GLIDEAPI api, const GlideOptions *options);
  VkCompareOp UtilCompareModeToVKCompareMode(GrCmpFnc_t fnc);
  VkCullModeFlags UtilCullModeVKCullModeFlags(GrCullMode_t mode);
  VkSamplerAddressMode UtilGlideClampToVKSamplerAddressMode(GrTextureClampMode_t mode);
  std::string UtilColorFormatToString(GrColorFormat_t format);
  std::string UtilInternalFormatToString(GrIntFmt_t format);
  GrIntFmt_t UtilTexFormatToInternalFormat(GrTextureFormat_t format);
  GrTextureFormat_t UtilInternalFormatToTexFormat(GrIntFmt_t format);
  GrLfbWriteMode_t UtilIFormatToLFBWriteMode(GrIntFmt_t format);
  GrIntFmt_t UtilLFBSrcFormatToIFormat(GrLfbSrcFmt_t format);
  GrIntFmt_t UtilLFBWriteModeToIFormat(GrLfbWriteMode_t writeMode);
  VkPrimitiveTopology UtilVertexModeToTopology(glide3x::GrDrawVertexArrayMode_t mode);
  GrColor_t UtilConvertColor(GrColorFormat_t format, FxFloat r, FxFloat g, FxFloat b, FxFloat a, FxBool isNormalized);
  void UtilGetColorComponent(GrColorFormat_t format, GrColor_t color, FxFloat* r, FxFloat* g, FxFloat* b, FxFloat* a, FxBool needNormalized);
  FxBool UtilClearImage(GrIntFmt_t format, GrColorFormat_t colorFormat, GrColor_t color, void* dstPtr, VkRect2D dimensions, VkRect2D region);
  FxBool UtilConvertRGBAToImage(GrIntFmt_t format, const void *srcPtr, void *dstPtr, FxU32 width, FxU32 height);
  FxBool UtilConvertImageToRGBA(GrIntFmt_t format, const void* srcPtr, void *dstPtr, FxU32& dstComponents, FxU32& dstPitch, FxU32 width, FxU32 height
    , FxBool isPalette6666 = FXFALSE, const std::array<FxU32, VOODOO_PALETTE_TABLE_SIZE> *palette = nullptr, GrColorFormat_t *colorFormat = nullptr);

}
