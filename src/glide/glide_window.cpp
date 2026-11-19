#include "glide_window.h"

namespace dxvk {
  static Rc<DxvkSwapchainBlitter> i_blitter = nullptr;
  static Rc<hud::Hud> i_hud = nullptr;
  static WNDPROC wndProc = nullptr;

  FxBool SetWindowHook(HWND window, Rc<DxvkSwapchainBlitter> blitter, Rc<hud::Hud> hud) {
    i_blitter = blitter;
    i_hud = hud;

    if (likely(window && wndProc == nullptr && i_blitter != nullptr && i_hud != nullptr))
      wndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HookedWindowProc)));

    return wndProc != nullptr;
  }

  LRESULT CALLBACK HookedWindowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (unlikely(i_blitter != nullptr && (GetKeyState(VK_SHIFT) & 0x8000))) {
      if (unlikely(i_hud && msg == WM_SYSKEYDOWN && wParam == VK_F10))
        i_hud->GetOptions()->opacity = std::max(i_hud->GetOptions()->opacity - 0.1f, 0.0f);
      else if (unlikely(i_hud && msg == WM_KEYDOWN && wParam == VK_F11))
        i_hud->GetOptions()->opacity = std::min(i_hud->GetOptions()->opacity + 0.1f, 1.0f);
      else if (unlikely(i_blitter && msg == WM_KEYDOWN && wParam == VK_F12)) {
        if (unlikely(i_hud && i_hud->GetOptions()->opacity == 0.0f))
          i_hud->GetOptions()->opacity = 0.1f;

        i_blitter->hudStateToggle();
      }

      return 0;
    }

    return CallWindowProcW(wndProc, window, msg, wParam, lParam);
  }

  BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    EnumWindowsData* data = reinterpret_cast<EnumWindowsData*>(lParam);
    DWORD wpid = 0;

    if (GetWindowThreadProcessId(hWnd, &wpid) && wpid == data->pid) {
      data->HWNDs.push_back(hWnd);
    }

    return TRUE;
  }

}
