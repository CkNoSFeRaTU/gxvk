#pragma once

#include "glide_include.h"
#include "glide_texture.h"

namespace dxvk {
  class GlideDevice;

  class GlideTMU final {

  public:
    using TMUAddress = FxU32;

    struct TMUTexture {
      TMUAddress startAddress = 0;
      TMUAddress stopAddress = 0;
      glide3x::GrLOD_t smallLodLog2 = glide3x::GR_LOD_LOG2_1;
      glide3x::GrLOD_t largeLodLog2 = glide3x::GR_LOD_LOG2_1;
      glide3x::GrAspectRatio_t aspectRatioLog2 = glide3x::GR_ASPECT_LOG2_1x1;
      GrTextureFormat_t format = GR_TEXFMT_8BIT;
      MipMapLevelMask_t evenOdd = GR_MIPMAPLEVELMASK_NONE;
      FxU32 width = 0;
      FxU32 height = 0;
      FxU32 size = 0;
      size_t hash = 0;

      FxBool upToDate = false;
      Rc<GlideCommonTexture> texture = nullptr;
    };

    struct TMUMetadata {
      TMUAddress startAddress = 0;
      glide3x::GrLOD_t smallLodLog2 = glide3x::GR_LOD_LOG2_1;
      glide3x::GrLOD_t largeLodLog2 = glide3x::GR_LOD_LOG2_1;
      glide3x::GrAspectRatio_t aspectRatioLog2 = glide3x::GR_ASPECT_LOG2_1x1;
      GrTextureFormat_t format = GR_TEXFMT_8BIT;
      MipMapLevelMask_t evenOdd = GR_MIPMAPLEVELMASK_NONE;
      FxU32 size = 0;

      uint8_t *data = nullptr;
    };

    GlideTMU(GlideDevice* pDevice, GrChipID_t id);
    ~GlideTMU();

    FxU32 GetMinimumAddress() {
      return 0;
    }

    FxU32 GetMaximumAddress() {
      return m_size;
    }

    GlideDevice* GetDevice() const {
      return m_parent;
    }

    FxBool InsertTextures(TMUMetadata metadata, size_t start = 0, size_t end = 0);
    FxBool InsertTextureLevel(TMUMetadata metadata, size_t start = 0, size_t end = 0);

    const TMUTexture* GetTexture(TMUMetadata& metadata);

    void SelectNCCTable(GrTexTable_t table);

    FxBool SetTable(GrTexTable_t type, void *data, FxU32 start, FxU32 stop);

    FxBool UploadTextures(TMUMetadata metadata);

    const std::array<FxU32, VOODOO_PALETTE_TABLE_SIZE>* GetPalette() const {
      return &m_palette;
    }

    force_inline void incRef() {
      m_refCount.fetch_add(1u);
    }

    force_inline void decRef() {
      if (m_refCount.fetch_sub(1u) == 1u)
        delete this;
    }

    struct {
      struct {
        GrCombineFunction_t alphaFunction;
        GrCombineFactor_t alphaFactor;
        FxBool alphaInvert;
        GrCombineFunction_t colorFunction;
        GrCombineFactor_t colorFactor;
        FxBool colorInvert;
      } texCombine;
      struct {
        FxI32 lodBias;
        FxU8 scale;
        float max;
      } texDetailControl;
      struct {
        GrTextureFilterMode_t min;
        GrTextureFilterMode_t mag;
      } texFilterMode;
      struct {
        GrMipMapMode_t mode;
        FxBool lodBlend;
      } texMipMapMode;
      struct {
        GrTextureClampMode_t s;
        GrTextureClampMode_t t;
      } texClampMode;
      float texLodBiasValue;
    } m_state;

  private:
    template <FxU32 N>
    size_t calculateHash(const std::array<FxU32, N>* arr) {
      std::string_view sv{reinterpret_cast<const char*>(arr->data()), arr->size() * sizeof(FxU32)};
      return std::hash<std::string_view>{}(sv);
    }

    GlideDevice*                      m_parent;
    FxI32                             m_id;
    FxU32                             m_revision;
    FxU32                             m_size;
    std::unique_ptr<uint8_t[]>        m_buffer = nullptr;
    std::atomic<uint32_t>             m_refCount = { 0u };
    GrTexTable_t                      m_currentNcc = GR_TEXTABLE_NCC0;
    std::array<FxU32,
      VOODOO_PALETTE_TABLE_SIZE>      m_palette = { };
    std::array<std::array<FxU32,
      VOODOO_NCC_TABLE_SIZE>, 2>      m_ncc = { };
    std::map<TMUAddress, TMUTexture>  m_regions = { };

    std::array<size_t, 3>             m_hashes = { };
  };
}
