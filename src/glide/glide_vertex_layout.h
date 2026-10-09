#pragma once

#include "glide_include.h"

#include <unordered_map>

#include "../dxvk/dxvk_constant_state.h"
#include "../util/util_flags.h"

namespace dxvk {

  constexpr std::array<glide3x::GrVertexLayoutParam_t, GLIDE_MAX_VERTEX_PARAM_COUNT> glideOrderedParams = {
    glide3x::GR_PARAM_XY,
    glide3x::GR_PARAM_Z,
    glide3x::GR_PARAM_W,
    glide3x::GR_PARAM_Q,
    glide3x::GR_PARAM_A,
    glide3x::GR_PARAM_RGB,
    glide3x::GR_PARAM_PARGB,
    glide3x::GR_PARAM_ST0,
    glide3x::GR_PARAM_ST1,
    glide3x::GR_PARAM_ST2,
    glide3x::GR_PARAM_Q0,
    glide3x::GR_PARAM_Q1,
    glide3x::GR_PARAM_Q2,
    glide3x::GR_PARAM_FOG_EXT,
  };

  using GlideVertexLayoutFlags = Flags<glide3x::GrVertexLayoutParam_t>;

  struct GlideVertex {
    std::unordered_map<glide3x::GrVertexLayoutParam_t, glide3x::GrVertexLayoutMode_t> modes;
    std::unordered_map<glide3x::GrVertexLayoutParam_t, glide3x::GrVertexLayoutOffset_t> offsets;
  };

  class GlideVertexLayout final {

  public:

    void GetLayout(GlideVertex *layout) {
      if (layout == nullptr)
        return;

      layout = &m_vertexLayout;
    }

    void SetLayout(const GlideVertex *layout) {
      if (layout == nullptr)
        return;

      m_vertexLayout = *layout;
      RecalculateVertexSize();
    }

    FxU32 GetLayoutSize() const {
      return sizeof(GlideVertex);
    }

    size_t GetCount() const {
      return m_count;
    }

    size_t GetOffset(glide3x::GrVertexLayoutParam_t param) {
      return m_vertexLayout.offsets[param];
    }

    size_t GetVertexSize() const {
      return m_vertexSize;
    }

    FxBool TestParam(glide3x::GrVertexLayoutParam_t param) const;

    FxBool SetVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutMode_t mode, glide3x::GrVertexLayoutOffset_t offset);

    std::array<DxvkVertexInput, GLIDE_MAX_VERTEX_PARAM_COUNT> GetAttributeList() {
      return m_vertexAttributeList;
    }

  private:
    void RecalculateVertexSize();

    GlideVertex                    m_vertexLayout = {};
    size_t                         m_vertexSize = 0;
    size_t                         m_count = 0;
    std::array<DxvkVertexInput, GLIDE_MAX_VERTEX_PARAM_COUNT> m_vertexAttributeList = {};


  };

}