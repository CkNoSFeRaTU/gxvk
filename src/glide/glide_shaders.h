#include "../dxvk/dxvk_shader.h"

#include "glide_include.h"
#include "glide_state.h"

#include "../dxvk/dxvk_shader.h"

namespace dxvk {

  class GlideDevice;

  class GlideShaderModuleSet : public RcObject {
    static constexpr uint32_t SamplerSet = 0u;
    static constexpr uint32_t SrvSet = 1u;
    static constexpr uint32_t CbvSet = 2u;
    static constexpr uint32_t SpecDataSet = 3u;
  public:

    GlideShaderModuleSet() = delete;

    explicit GlideShaderModuleSet(GlideDevice* pDevice);

    template<GlideShaderType Stage>
    Rc<DxvkShader> GetShader() {
      return Stage == GlideShaderType::VertexShader ? m_vs : m_fs;
    }

  private:

    Rc<DxvkShader> m_vs;
    Rc<DxvkShader> m_fs;

    static Rc<DxvkShader> buildVs();
    static Rc<DxvkShader> buildFs(GlideDevice* pDevice);

    constexpr static uint32_t GetPushSamplerOffset(uint32_t samplerIndex) {
      // Located directly after the PS push data block.
      return MaxSharedPushDataSize +
        sizeof(GlideFfpsPushData) +
        sizeof(uint16_t) * samplerIndex;
    }

  };

}