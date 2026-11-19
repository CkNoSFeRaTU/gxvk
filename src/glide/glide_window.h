#pragma once

#include "glide_include.h"

#include "../dxvk/dxvk_swapchain_blitter.h"

namespace dxvk {
  struct EnumWindowsData {
    DWORD pid;
    std::vector<HWND> HWNDs;
  };

  BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam);
  LRESULT CALLBACK HookedWindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
  FxBool SetWindowHook(HWND window, Rc<DxvkSwapchainBlitter> blitter, Rc<hud::Hud> hud);

}
