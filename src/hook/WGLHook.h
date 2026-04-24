#pragma once
#include <Windows.h>

#include <imgui/imgui.h>

class WGLHook {
public:
    static void init();
    static bool mInitialized;
    static HWND mHwnd;
    static WNDPROC mOriginalWndProc;
};
