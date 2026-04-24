#include "WGLHook.h"
#include <Windows.h>
#include <windowsx.h>
#include "include/imgui/imgui.h"
#include "include/imgui/backends/imgui_impl_win32.h"
#include "include/imgui/backends/imgui_impl_opengl3.h"
#include <MinHook.h>
#include "src/features/FeatureManager.h"
#include "src/features/event/RenderEvent.h"
#include "src/features/event/KeyEvent.h"
#include "src/features/event/MouseEvent.h"
#include "src/utils/Logger.h"

bool WGLHook::mInitialized = false;
HWND WGLHook::mHwnd = nullptr;
WNDPROC WGLHook::mOriginalWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void handleKeyboard(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN || msg == WM_KEYUP) {
        int vkey = (int)wParam;
        if (vkey == VK_SHIFT) {
            UINT scancode = (lParam & 0x00ff0000) >> 16;
            vkey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
        }
        else if (vkey == VK_CONTROL) {
            vkey = (lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
        }
        else if (vkey == VK_MENU) {
            vkey = (lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
        }


        bool pressed = (msg == WM_KEYDOWN);
        auto holder = nes::make_holder<KeyEvent>(vkey, pressed);
        gFeatureManager->mDispatcher->trigger(holder);

        if (pressed) {
            for (auto& module : gFeatureManager->mModuleManager->mModules) {
                if (module->getKey() != 0 && module->getKey() == vkey) {
                    module->toggle();
                }
            }
        }
    }
}


static void handleMouse(UINT msg, WPARAM wParam, LPARAM lParam) {
    float x = (float)GET_X_LPARAM(lParam);
    float y = (float)GET_Y_LPARAM(lParam);
    switch (msg) {
    case WM_LBUTTONDOWN: case WM_LBUTTONUP: {
        auto holder = nes::make_holder<MouseEvent>(0, msg == WM_LBUTTONDOWN, x, y);
        gFeatureManager->mDispatcher->trigger(holder);
        break;
    }
    case WM_RBUTTONDOWN: case WM_RBUTTONUP: {
        auto holder = nes::make_holder<MouseEvent>(1, msg == WM_RBUTTONDOWN, x, y);
        gFeatureManager->mDispatcher->trigger(holder);
        break;
    }
    case WM_MBUTTONDOWN: case WM_MBUTTONUP: {
        auto holder = nes::make_holder<MouseEvent>(2, msg == WM_MBUTTONDOWN, x, y);
        gFeatureManager->mDispatcher->trigger(holder);
        break;
    }
    }
}

LRESULT CALLBACK hookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

    handleKeyboard(msg, wParam, lParam);
    handleMouse(msg, wParam, lParam);

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        switch (msg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP:
        case WM_MOUSEWHEEL: case WM_MOUSEMOVE:
            return 1;
        }
    }

    return CallWindowProcA(WGLHook::mOriginalWndProc, hWnd, msg, wParam, lParam);
}

typedef BOOL(WINAPI* wglSwapBuffers_t)(HDC);
static wglSwapBuffers_t originalSwapBuffers = nullptr;

BOOL WINAPI hookedSwapBuffers(HDC hDc) {
    if (!WGLHook::mInitialized) {
        ImGui::CreateContext();

        WGLHook::mHwnd = WindowFromDC(hDc);
        ImGui_ImplWin32_Init(WGLHook::mHwnd);
        ImGui_ImplOpenGL3_Init("#version 130");

        WGLHook::mOriginalWndProc = (WNDPROC)SetWindowLongPtrA(
            WGLHook::mHwnd, GWLP_WNDPROC, (LONG_PTR)hookedWndProc
        );

        WGLHook::mInitialized = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	RenderEvent event;

	auto holder = nes::make_holder<RenderEvent>(event);
	gFeatureManager->mDispatcher->trigger(holder);

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return originalSwapBuffers(hDc);
}

void WGLHook::init() {
    void* target = GetProcAddress(GetModuleHandleA("opengl32.dll"), "wglSwapBuffers");
    MH_CreateHook(target, &hookedSwapBuffers, (void**)&originalSwapBuffers);
    MH_EnableHook(target);
}