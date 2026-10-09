#pragma once

#include "glide_include_common.h"

namespace glide1x {
  // TODO: state stub
  constexpr ::FxI32 GLIDE_STATE_PAD_SIZE = 272;
  typedef struct _GrState_s {
    char pad[GLIDE_STATE_PAD_SIZE];
  } GrState;
}