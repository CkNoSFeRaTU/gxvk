#include "../util/util_math.h"

#include "glide_options.h"

namespace dxvk {

  GlideOptions::GlideOptions(const Config& config) {
    this->presentInterval = config.getOption<int32_t>("glide.presentInterval", -1);
    this->deviceReset = config.getOption<bool>("glide.deviceReset", true);
    this->multithread = config.getOption<bool>("glide.multithread", false);
    this->useMonitorResolution = config.getOption<bool>("glide.useMonitorResolution", false);

    this->ignoreGammaCorrection = config.getOption<bool>("glide.ignoreGammaCorrection", false);

    this->hackNoBlend = config.getOption<bool>("glide.hackNoBlend", false);
    this->hackNoClears = config.getOption<bool>("glide.hackNoClears", false);
    this->hackNoDepth = config.getOption<bool>("glide.hackNoDepth", false);
    this->hackNoLFB = config.getOption<bool>("glide.hackNoLFB", false);

    std::string card = Config::toLower(config.getOption<std::string>("glide.card", "auto"));
    if (card == "auto") {
      this->card = GlideCard::CARD_AUTO;
    } else if (card == "voodoo" || card == "sst-1") {
      this->card = GlideCard::CARD_VOODOO;
    } else if (card == "rush" || card == "sst-96") {
      this->card = GlideCard::CARD_RUSH;
    } else if (card == "voodoo2" || card == "cvg") {
      this->card = GlideCard::CARD_VOODOO2;
    } else if (card == "banshee") {
      this->card = GlideCard::CARD_BANSHEE;
    } else if (card == "voodoo3") {
      this->card = GlideCard::CARD_VOODOO3;
    } else if (card == "voodoo4") {
      this->card = GlideCard::CARD_VOODOO4;
    } else if (card == "voodoo5") {
      this->card = GlideCard::CARD_VOODOO5;
    }

    this->maxFrameRate = config.getOption<int32_t>("dxvk.maxFrameRate",
                         config.getOption<int32_t>("glide.maxFrameRate", 0));
  }

}
