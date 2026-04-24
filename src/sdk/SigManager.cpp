#include "SigManager.h"

#include <map>
#include <windows.h>
#include <psapi.h>
#include <utils/Logger.h>
#include <unordered_map>
#include "Main.h"
#include <unordered_set>
#include "include/magic_enum/magic_enum.hpp"

struct MemoryRegion {
    uintptr_t base;
    size_t size;
};

static MemoryRegion get_text_section(const char* moduleName) {
    uintptr_t hModule = (uintptr_t)GetModuleHandleA(moduleName);
    if (!hModule) return { 0, 0 };

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(hModule + dosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);

    for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
        // 判断是否为代码段（通常名为 .text）
        if (strcmp((const char*)section[i].Name, ".text") == 0) {
            return { hModule + section[i].VirtualAddress, section[i].Misc.VirtualSize };
        }
    }

    // 如果找不到 .text，作为兜底返回整个模块大小
    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), (HMODULE)hModule, &mi, sizeof(mi));
    return { (uintptr_t)mi.lpBaseOfDll, (size_t)mi.SizeOfImage };
}

struct ScanTask {
    SignatureID id;
    SignatureType type;
    int offset;
    std::vector<uint8_t> bytes;
    std::vector<uint8_t> mask;
};

#include <sstream>
#include <set>

static void parse_pattern(const std::string& pattern, std::vector<uint8_t>& bytes, std::vector<uint8_t>& mask) {
    std::stringstream ss(pattern);
    std::string hex;
    while (ss >> hex) {
        if (hex == "?" || hex == "??") {
            bytes.push_back(0);
            mask.push_back(0); // 0 代表通配符，扫描时忽略
        }
        else {
            bytes.push_back((uint8_t)std::strtol(hex.c_str(), nullptr, 16));
            mask.push_back(0xFF); // 0xFF 代表必须完全匹配
        }
    }
}

static uintptr_t finalize_address(uintptr_t addr, SignatureType type, int offset) {
    switch (type) {
    case SignatureType::Call:
        // x64 Call: E8 <4字节相对偏移>
        // 目标地址 = call指令地址 + 指令长度(5) + 读取到的32位偏移
        return addr + 5 + *(int32_t*)(addr + 1) + offset;

    case SignatureType::Lea:
        // x64 Lea: 48 8D 05 <4字节相对偏移> (通常是7字节)
        // 目标地址 = lea指令地址 + 指令长度(7) + 读取到的32位偏移
        return addr + 7 + *(int32_t*)(addr + 3) + offset;

    case SignatureType::Direct:
    case SignatureType::Offset:
        return addr + offset;

    default:
        return addr;
    }
}


// 存放任务清单（待扫描的信息）
static std::vector<SignatureInfo> signature_tasks;

// 存放扫描后的结果（ID -> 绝对地址）
static std::map<SignatureID, uintptr_t> m_cache;

// 基础版：单特征码
static void add_signature(SignatureID id, std::string pattern, SignatureType type) {
    signature_tasks.emplace_back(id, type, std::move(pattern), 0);
}

// 带偏移量版
static void add_signature(SignatureID id, std::string pattern, int offset, SignatureType type) {
    signature_tasks.emplace_back(id, type, std::move(pattern), offset);
}

// 多特征码备选版 (参考你喜欢的那个设计)
static void add_signature(SignatureID id, std::initializer_list<std::string> patterns, SignatureType type, int offset = 0) {
    // 这里需要给 SignatureInfo 增加一个支持 initializer_list 的构造函数
    signature_tasks.emplace_back(id, type, patterns, offset);
}

void SigManager::init() {
    int64_t preInit = NOW;
    add_signature(SignatureID::Actor_getPosition,
        { "E8 ?? ?? ?? ?? 0F B6 4F ?? 84 C9",
        "E8 ?? ?? ?? ?? F3 0F 10 45 ?? 48 8B CF",
        "E8 ?? ?? ?? ?? F3 45 0F 58 C0 F3 0F 58 FF"},
        SignatureType::Call);

    add_signature(SignatureID::Actor_getLevel, {
        "E8 ?? ?? ?? ?? 4C 8B C8 45 33 C0 48 8B D7",
        "E8 ?? ?? ?? ?? ?? ?? ?? 4C 8B F0 45 33 C0",
        "E8 ?? ?? ?? ?? 48 8B D8 49 8B CF E8 ?? ?? ?? ?? 48 8B F0"},
        SignatureType::Call);

    add_signature(SignatureID::Actor_normalTick, 
        "48 8B C4 48 89 58 ?? 48 89 70 ?? 48 89 78 ?? "
        "55 41 54 41 55 41 56 41 57 48 8D A8 ?? ?? ?? "
        "?? 48 81 EC ?? ?? ?? ?? 0F 29 70 ?? 0F 29 78 "
        "?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 85 ?? "
        "?? ?? ?? 48 8B F9 45 33 ED 41 8B F5", 
        SignatureType::Direct);

    add_signature(SignatureID::Actor_getRegion,
        "40 53 48 83 EC ?? 48 8B 91 ?? ?? ?? "
        "?? 45 33 C0",
        SignatureType::Direct);

    add_signature(SignatureID::Actor_getDimension,
        "40 53 48 83 EC ?? 48 8B 91 ?? ?? ?? ?? "
        "48 8B D9 48 85 D2 74 ?? 8B 42",
        SignatureType::Direct);

    add_signature(SignatureID::Actor_getFallDistance, 
        "E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
        "F2 0F 11 7C 24", 
        SignatureType::Call);

    add_signature(SignatureID::Player_getGameMode, {
        "E8 ?? ?? ?? ?? 4C 8B C8 4D 8D 47",
        "E8 ?? ?? ?? ?? 48 8B C8 45 84 F6",
        "E8 ?? ?? ?? ?? 44 8B C3 48 8B CE"},
        SignatureType::Call);

    add_signature(SignatureID::Player_getSupplies, {
    "E8 ?? ?? ?? ?? 8B 57 ?? 45 33 C0",
    "E8 ?? ?? ?? ?? 40 84 FF 75 ?? B8",
    "E8 ?? ?? ?? ?? 45 33 C0 41 8B 54 24" },
    SignatureType::Call);

    add_signature(SignatureID::LocalPlayer_swing,
        "40 53 48 81 EC ?? ?? ?? ?? 48 8B 05 "
        "?? ?? ?? ?? 48 33 C4 48 89 44 24 ?? "
        "48 8B D9 E8 ?? ?? ?? ?? 4C 8B 53",
        SignatureType::Direct);
    add_signature(SignatureID::ClientInstance_update,
        "48 8B C4 48 89 58 ?? 48 89 70 ?? 48 "
        "89 78 ?? 55 41 54 41 55 41 56 41 57 "
        "48 8D A8 ?? ?? ?? ?? 48 81 EC ?? ?? "
        "?? ?? 0F 29 70 ?? 48 8B 05 ?? ?? ?? "
        "?? 48 33 C4 48 89 85 ?? ?? ?? ?? 0F B6 F2",
        SignatureType::Direct);

    add_signature(SignatureID::Minecraft_update,
        "48 8B C4 48 89 58 ?? 48 89 70 ?? 48 "
        "89 78 ?? 55 41 54 41 55 41 56 41 57 "
        "48 8D A8 ?? ?? ?? ?? 48 81 EC ?? ?? "
        "?? ?? 0F 29 70 ?? 0F 29 78 ?? 48 8B "
        "05 ?? ?? ?? ?? 48 33 C4 48 89 85 ?? "
        "?? ?? ?? 4C 8B E1 48 89 4C 24", 
        SignatureType::Direct);

    add_signature(SignatureID::LoopbackPacketSender_sendToServer,
        "48 89 5C 24 ?? 57 48 81 EC ?? ?? ?? "
        "?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 "
        "89 84 24 ?? ?? ?? ?? 0F B6 41",
        SignatureType::Direct);

    add_signature(SignatureID::MinecraftPackets_createPacket,
        "48 89 5C 24 ?? 48 89 74 24 ?? 48 89 "
        "7C 24 ?? 55 41 56 41 57 48 8B EC 48 "
        "83 EC ?? 48 8B F1 48 89 4D",
        SignatureType::Direct);
    
    //add_signature(SignatureID::Json_FastWriter_write,
    //    "48 89 5C 24 ?? 48 89 54 24 ?? 57 48 "
    //    "83 EC ?? 48 8D 59 ?? 48 8B FA",
    //    SignatureType::Direct);

    // 2. 动态获取代码段范围
    MemoryRegion text = get_text_section("Minecraft.Windows.exe");
    if (text.base == 0) return;

    // 3. 预处理：解析字符串并分类
    std::vector<ScanTask> tasks;
    for (const auto& task : signature_tasks) {
        for (const auto& pat : task.mPatterns) {
            ScanTask st{ task.mId, task.mType, task.mOffset };
            // 这里调用解析函数将 "E8 ?? ..." 转为 bytes 和 mask
            parse_pattern(pat, st.bytes, st.mask);
            tasks.push_back(std::move(st));
        }
    }

    Logger::info("text section size: %zu MB", text.size / 1024 / 1024);
    Logger::info("tasks count: %zu", tasks.size());

    std::vector<std::vector<ScanTask*>> firstByteIndex(256);
    for (auto& task : tasks) {
        if (!task.bytes.empty() && task.mask[0] == 0xFF) {
            firstByteIndex[task.bytes[0]].push_back(&task);
        }
    }

    uint8_t* start = reinterpret_cast<uint8_t*>(text.base);
    uint8_t* end = start + text.size;
    std::unordered_set<int> completed_ids;
    size_t total = tasks.size();

    for (uint8_t* curr = start; curr < end; ++curr) {
        auto& bucket = firstByteIndex[*curr];
        if (bucket.empty()) continue;

        for (size_t i = 0; i < bucket.size(); ) {
            ScanTask* task = bucket[i];

            if (completed_ids.count((int)task->id)) {
                bucket[i] = bucket.back();
                bucket.pop_back();
                continue;
            }

            if (curr + task->bytes.size() > end) { ++i; continue; }

            bool match = true;
            for (size_t j = 0; j < task->bytes.size(); ++j) {
                if ((curr[j] & task->mask[j]) != task->bytes[j]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                uintptr_t found_addr = reinterpret_cast<uintptr_t>(curr);
                uintptr_t final_addr = finalize_address(found_addr, task->type, task->offset);
                m_cache[task->id] = final_addr;
                Logger::info("Found Signature: [ID: %d] at %p -> Final: %p",
                    static_cast<int>(task->id), found_addr, final_addr);
                completed_ids.insert((int)task->id);
                bucket[i] = bucket.back();
                bucket.pop_back();
                continue;
            }
            ++i;
        }
        if (completed_ids.size() == total) break;
    }

    // --- 在循环结束后添加：未找到的检查日志 ---
    for (const auto& task_info : signature_tasks) {
        if (completed_ids.find((int)task_info.mId) == completed_ids.end()) {
            Logger::error("CRITICAL: Failed to find signature for [ID: %d] [Name: %s]!",
                static_cast<int>(task_info.mId),
                magic_enum::enum_name(static_cast<SignatureID>(task_info.mId)).data());
        }
    }

    Logger::info("scan signature time %d",NOW - preInit);
}

std::optional<uintptr_t> SigManager::get(SignatureID id) {
    auto it = m_cache.find(id);
    if (it != m_cache.end()) {
        return it->second;
    }
    return std::nullopt;
}