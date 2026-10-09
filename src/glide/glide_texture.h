#pragma once

#include "../util/util_small_vector.h"

#include "../dxvk/dxvk_cs.h"
#include "../dxvk/dxvk_device.h"

#include "glide_include.h"

namespace dxvk {

  class GlideDevice;

  struct GLIDE_COMMON_TEXTURE_DESC {
    UINT             Id;
    UINT             Width;
    UINT             Height;
    UINT             MipMaps;
    VkFormat         Format;
    GrBuffer_t       Type;
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

    force_inline void incRef() {
      m_refCount.fetch_add(1u);
    }

    force_inline void decRef() {
      if (m_refCount.fetch_sub(1u) == 1u)
        delete this;
    }

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

  };

}
