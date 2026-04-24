// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "windows.h"
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include "src/utils/stdint.h"
#include "Main.h"
#include <tlhelp32.h>

DWORD WINAPI HackThread(LPVOID param) {
    // 获取当前进程exe名
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string fullPath(path);

    // 截取文件名
    std::string exeName = fullPath.substr(fullPath.rfind('\\') + 1);

    if (exeName != "Minecraft.Windows.exe") {
        MessageBoxA(nullptr, "不是MC！", "MCHook", MB_OK);
        return 0;
    }

    MessageBoxA(nullptr, "注入成功！", "MCHook", MB_OK);

    Main::init((HMODULE)param);

    return 0;
}

bool __stdcall DllMain(const HMODULE hModule, const DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        Main::hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, HackThread, hModule, 0, nullptr);
    }
    return true;
}