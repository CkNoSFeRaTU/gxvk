#pragma once

#include "../util/util_flags.h"
#include "../util/util_small_vector.h"
#include "../util/util_unmap.h"

#include "../dxvk/dxvk_cs.h"
#include "../dxvk/dxvk_device.h"

#include "glide_include.h"

namespace dxvk {

  class GlideDevice;

  struct GLIDE_COMMON_TEXTURE_DESC {
    std::string      Id;
    UINT             Width;
    UINT             Height;
    UINT             MipMaps;
    VkFormat         Format;
    GrBuffer_t       Type;
  };

  enum GlideTextureDirty : int {
    TEXTURE_UPTODATE = 0,
    TEXTURE_DIRTY_GPU,
    TEXTURE_DIRTY_CPU,
  };

  class GlideCommonTexture {
  public:
    GlideCommonTexture(
            GlideDevice*                pDevice,
      const GLIDE_COMMON_TEXTURE_DESC*  pDesc);

    ~GlideCommonTexture();

    void CreateSampleView(UINT Lod);

    Rc<DxvkImage> GetImage() const {
      return m_image;
    }

    Rc<DxvkImageView> CreateView(
          UINT                   Lod,
          VkImageUsageFlags      UsageFlags,
          VkImageLayout          Layout);

    const Rc<DxvkImageView>& GetView(UINT Lod);

    void CreateBuffer(bool Initialize, uint32_t Size);

    force_inline void incRef() {
      m_refCount.fetch_add(1u);
    }

    force_inline void decRef() {
      if (m_refCount.fetch_sub(1u) == 1u)
        delete this;
    }

    void SetDirty(GlideTextureDirty state) {
      m_dirty = state;
    }

    FxBool IsDirty(GlideTextureDirty state) const {
      return m_dirty != TEXTURE_UPTODATE && m_dirty != state;
    }

    void Clear(FxU32 color, VkRect2D region, FxBool partial);

    FxU32 GetStride() const {
      return m_desc.Width * m_blockSize;
    }

    void ConsiderDownload();
    void ConsiderUpload();
    void *Lock(GrLock_t type, GrOriginLocation_t& origin, GrLfbWriteMode_t& mode);
    FxBool Unlock(GrLock_t type);

    inline void Swap(GlideCommonTexture* Other) {
      // Only used for swap chain back buffers that don't
      // have a container and all have identical properties
      std::swap(m_image, Other->m_image);
      std::swap(m_view, Other->m_view);
    }

  private:
    GlideDevice*                  m_device;
    GLIDE_COMMON_TEXTURE_DESC     m_desc;

    std::atomic<uint32_t>         m_refCount = { 0u };
    Rc<DxvkImage>                 m_image = nullptr;
    Rc<DxvkImageView>             m_view = nullptr;

    GrOriginLocation_t            m_origin = GR_ORIGIN_UPPER_LEFT;
    FxU32                         m_blockSize = 0u;
    GlideTextureDirty             m_dirty = TEXTURE_UPTODATE;
    bool                          m_isLocked = false;
    GrLfbWriteMode_t              m_lockmode = GR_LFBWRITEMODE_565;
    Rc<DxvkBuffer>                m_buffer;
    MemoryFileRegion              m_data = { };
    size_t                        m_totalSize = 0u;
    std::vector<uint8_t>          m_convertBuffer = { };
  };

}
