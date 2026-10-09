#pragma once

#include "../util/config/config.h"
#include "../dxvk/dxvk_device.h"

namespace dxvk {
  enum class GlideCard : uint8_t {
    CARD_AUTO = 0,
    CARD_VOODOO = 1,   // SST-1
    CARD_RUSH  = 2,    // SST-96
    CARD_VOODOO2 = 3,  // CVG
    CARD_BANSHEE = 4,  // H3
    CARD_VOODOO3 = 5,  // H3
    CARD_VOODOO4 = 6,  // H5
    CARD_VOODOO5 = 7,  // H5
  };

  struct GlideOptions {
    GlideOptions(const Config& config);

    GlideCard card;
    int32_t   presentInterval;
    bool      deviceReset;
    bool      multithread;
    bool      useMonitorResolution;

    bool      ignoreGammaCorrection;

    bool      hackNoBlend;
    bool      hackNoClears;
    bool      hackNoDepth;
    bool      hackNoLFB;

    int32_t   maxFrameRate;
  };

}
