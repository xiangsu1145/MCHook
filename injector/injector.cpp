#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ntdll.lib")

bool FindProcessByName(const char* name, DWORD& outPid) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &pe)) {
        do {
            std::wstring exeName = pe.szExeFile;
            std::string exeNameA(exeName.begin(), exeName.end());
            if (exeNameA.find(name) != std::string::npos) {
                outPid = pe.th32ProcessID;
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32NextW(snapshot, &pe));
    }

    CloseHandle(snapshot);
    return false;
}

bool InjectDll(DWORD pid, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        std::cerr << "[ERROR] Cannot open process: " << GetLastError() << std::endl;
        return false;
    }

    size_t dllPathLen = strlen(dllPath) + 1;
    LPVOID remotePath = VirtualAllocEx(hProcess, nullptr, dllPathLen, MEM_COMMIT, PAGE_READWRITE);
    if (!remotePath) {
        std::cerr << "[ERROR] VirtualAllocEx failed: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    if (!WriteProcessMemory(hProcess, remotePath, dllPath, dllPathLen, nullptr)) {
        std::cerr << "[ERROR] WriteProcessMemory failed: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!loadLibAddr) {
        std::cerr << "[ERROR] GetProcAddress failed: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
        (LPTHREAD_START_ROUTINE)loadLibAddr, remotePath, 0, nullptr);
    if (!hThread) {
        std::cerr << "[ERROR] CreateRemoteThread failed: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    VirtualFreeEx(hProcess, remotePath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return true;
}

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "        MCHook Injector" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;

    const char* dllName = "MCHook.dll";
    char dllPath[MAX_PATH];
    GetModuleFileNameA(NULL, dllPath, MAX_PATH);
    char* p = strrchr(dllPath, '\\');
    if (p) *p = '\0';
    strcat_s(dllPath, MAX_PATH, "\\");
    strcat_s(dllPath, MAX_PATH, dllName);

    std::cout << "[INFO] Looking for Minecraft..." << std::endl;

    DWORD pid = 0;
    if (!FindProcessByName("Minecraft.Windows.exe", pid)) {
        std::cerr << "[ERROR] Minecraft not found!" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "[INFO] Found Minecraft! PID: " << pid << std::endl;
    std::cout << "[INFO] DLL Path: " << dllPath << std::endl;
    std::cout << "[INFO] Injecting..." << std::endl;

    if (InjectDll(pid, dllPath)) {
        std::cout << std::endl;
        std::cout << "[SUCCESS] Injected successfully!" << std::endl;
    }
    else {
        std::cerr << std::endl;
        std::cerr << "[FAIL] Injection failed!" << std::endl;
    }

    return 0;
}
