#pragma once
#include <imgui/imgui.h>
#include <cmath>

class ColorUtils {
public:
    // 主题色
    static inline ImVec4 Primary = ImVec4(0.00f, 0.47f, 0.83f, 1.0f); // #0078D4
    static inline ImVec4 PrimaryHov = ImVec4(0.06f, 0.39f, 0.75f, 1.0f); // #106EBE
    static inline ImVec4 PrimaryLight = ImVec4(0.91f, 0.96f, 0.99f, 1.0f); // #E8F4FD
    static inline ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static inline ImVec4 BgPanel = ImVec4(0.97f, 0.97f, 0.98f, 1.0f); // #F7F7F9
    static inline ImVec4 BgHeader = ImVec4(0.00f, 0.47f, 0.83f, 1.0f); // same as primary
    static inline ImVec4 Border = ImVec4(0.88f, 0.88f, 0.90f, 1.0f); // #E1E1E6
    static inline ImVec4 TextPrimary = ImVec4(0.10f, 0.10f, 0.12f, 1.0f); // #1A1A1F
    static inline ImVec4 TextMuted = ImVec4(0.47f, 0.47f, 0.51f, 1.0f); // #787882
    static inline ImVec4 ModuleOn = ImVec4(0.00f, 0.47f, 0.83f, 1.0f);
    static inline ImVec4 ModuleOff = ImVec4(0.93f, 0.93f, 0.95f, 1.0f);
    static inline ImVec4 Danger = ImVec4(0.82f, 0.20f, 0.22f, 1.0f);

    static ImU32 toU32(ImVec4 col) {
        return ImGui::ColorConvertFloat4ToU32(col);
    }

    static ImVec4 lerp(ImVec4 a, ImVec4 b, float t) {
        return ImVec4(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        );
    }

    static ImVec4 withAlpha(ImVec4 col, float alpha) {
        return ImVec4(col.x, col.y, col.z, alpha);
    }

    static ImVec4 darken(ImVec4 col, float amount) {
        return ImVec4(
            (std::max)(0.0f, col.x - amount),
            (std::max)(0.0f, col.y - amount),
            (std::max)(0.0f, col.z - amount),
            col.w
        );
    }
};
