#include "glide_vertex_layout.h"
#include "glide_include.h"

#include <unordered_map>

namespace dxvk {

  FxBool GlideVertexLayout::TestParam(glide3x::GrVertexLayoutParam_t param) const {
    auto it = m_vertexLayout.modes.find(param);
    if (it != m_vertexLayout.modes.end())
      return it->second == glide3x::GR_PARAM_ENABLE;

    return FXFALSE;
  }

  FxBool GlideVertexLayout::SetVertexLayout(glide3x::GrVertexLayoutParam_t param, glide3x::GrVertexLayoutMode_t mode, glide3x::GrVertexLayoutOffset_t offset) {
    if (unlikely(m_vertexLayout.modes[param] == mode && m_vertexLayout.offsets[param] == offset))
      return FXFALSE;

    m_vertexLayout.modes[param] = mode;
    m_vertexLayout.offsets[param] = offset;

    RecalculateVertexSize();

    return FXTRUE;
  }

  void GlideVertexLayout::RecalculateVertexSize() {
    m_count = 0;
    m_vertexAttributeList.fill({});

    for (std::size_t i = 0; i < glideOrderedParams.size(); ++i) {
      const auto& param = glideOrderedParams[i];
      if (m_vertexLayout.modes[param] == glide3x::GR_PARAM_ENABLE) {
        Logger::debug(str::format("Vertex layout attribute enabling param: ", param));
        DxvkVertexAttribute attrib = {};
        switch (param) {
          case glide3x::GR_PARAM_XY:
            attrib.format = VK_FORMAT_R32G32_SFLOAT;
            break;
          case glide3x::GR_PARAM_W:
          case glide3x::GR_PARAM_Z:
          case glide3x::GR_PARAM_A:
          case glide3x::GR_PARAM_Q:
          case glide3x::GR_PARAM_Q0:
          case glide3x::GR_PARAM_Q1:
          case glide3x::GR_PARAM_Q2:
          case glide3x::GR_PARAM_FOG_EXT:
            attrib.format = VK_FORMAT_R32_SFLOAT;
            break;
          case glide3x::GR_PARAM_PARGB:
            attrib.format = VK_FORMAT_R8G8B8A8_UINT;
            break;
          case glide3x::GR_PARAM_RGB:
            attrib.format = VK_FORMAT_R32G32B32_SFLOAT;
            break;
          case glide3x::GR_PARAM_ST0:
          case glide3x::GR_PARAM_ST1:
          case glide3x::GR_PARAM_ST2:
            attrib.format = VK_FORMAT_R32G32_SFLOAT;
            break;
        }
        attrib.binding = 0;
        attrib.offset = m_vertexLayout.offsets[param];
        attrib.location = i;
        m_vertexAttributeList[m_count++] = DxvkVertexInput(attrib);
      }
    }

    for (size_t i = m_count; i < GLIDE_MAX_VERTEX_PARAM_COUNT; i++) {
        DxvkVertexAttribute attrib = {};
        attrib.binding = GLIDE_MAX_TEXTURESTAGES;
        attrib.location = i;
        attrib.offset = 0;
        m_vertexAttributeList[i] = DxvkVertexInput(attrib);
    }

    glide3x::GrVertexLayoutOffset_t maxOffset = 0;
    glide3x::GrVertexLayoutParam_t maxParam = glide3x::GR_PARAM_XY;
    for (auto& [param, offset] : m_vertexLayout.offsets) {
        if (m_vertexLayout.modes[param] == glide3x::GR_PARAM_ENABLE && maxOffset <= offset) {
            maxOffset = offset;
            maxParam = param;
        }
    }

    FxU32 size = 0;
    switch (maxParam) {
        case glide3x::GR_PARAM_Z:
        case glide3x::GR_PARAM_W:
        case glide3x::GR_PARAM_A:
        case glide3x::GR_PARAM_Q:
        case glide3x::GR_PARAM_Q0:
        case glide3x::GR_PARAM_Q1:
        case glide3x::GR_PARAM_Q2:
        case glide3x::GR_PARAM_PARGB:
        case glide3x::GR_PARAM_FOG_EXT:
            size = 4;
            break;
        case glide3x::GR_PARAM_XY:
            size = 8;
            break;
        case glide3x::GR_PARAM_RGB:
        case glide3x::GR_PARAM_ST0:
        case glide3x::GR_PARAM_ST1:
        case glide3x::GR_PARAM_ST2:
            size = 12;
            break;
        default:
            break;
    }

    m_vertexSize = maxOffset + size;
    Logger::debug(str::format("Calculated vertex layout size: ", m_vertexSize, " vs ", sizeof(glide2x::GrVertex)));
  }

}