#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

namespace RenderUtils {

    // --- 1. 基础圆角填充 (带 Alpha 继承) ---
    inline void DrawRectFilled(const ImVec2& pos, const ImVec2& size, ImU32 color, float rounding = 10.0f, ImDrawFlags flags = 0) {
        ImGui::GetWindowDrawList()->AddRectFilled(pos, pos + size, color, rounding, flags);
    }

    // --- 2. 文本居中绘制 (UI 灵魂) ---
    // 自动计算文本宽度并将其放置在父容器正中心
    inline void DrawTextCentered(const char* text, const ImVec2& pos, const ImVec2& size, ImU32 color, ImFont* font = nullptr) {
        if (font) ImGui::PushFont(font);

        ImVec2 textSize = ImGui::CalcTextSize(text);
        ImVec2 textPos = ImVec2(
            pos.x + (size.x - textSize.x) * 0.5f,
            pos.y + (size.y - textSize.y) * 0.5f
        );

        ImGui::GetWindowDrawList()->AddText(textPos, color, text);

        if (font) ImGui::PopFont();
    }

    // --- 3. NL 风格卡片绘制 (封装你的 AddShadowRect) ---
    // 这是你明天画功能面板的核心函数
    inline void DrawCard(const ImVec2& pos, const ImVec2& size, float alpha, float rounding = 12.0f) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 1. 绘制阴影 (利用你的函数)
        // NL 的阴影通常很大但很淡：thickness=15.0f, offset=(0, 5)
        drawList->AddShadowRect(
            pos, pos + size,
            ColorUtils::applyAlpha(ColorUtils::Palette::Shadow, alpha),
            15.0f, ImVec2(0, 5), 0, rounding
        );

        // 2. 绘制白色主体
        drawList->AddRectFilled(
            pos, pos + size,
            ColorUtils::applyAlpha(ColorUtils::Palette::White, alpha),
            rounding
        );

        // 3. 绘制极细边框 (增加精致感)
        drawList->AddRect(
            pos, pos + size,
            ColorUtils::applyAlpha(ColorUtils::Palette::FrameOutline, alpha),
            rounding, 0, 1.0f
        );
    }

    // --- 4. 渐变条绘制 (用于选中状态或进度条) ---
    inline void DrawGradientBar(const ImVec2& pos, const ImVec2& size, float alpha) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 col1 = ColorUtils::applyAlpha(ColorUtils::Palette::AccentBlue, alpha);
        ImU32 col2 = ColorUtils::applyAlpha(IM_COL32(0, 200, 255, 255), alpha); // 稍微亮一点的蓝

        drawList->AddRectFilledMultiColor(
            pos, pos + size,
            col1, col2, col2, col1 // 左右渐变
        );
    }

    // --- 5. 裁剪区域管理 (用于子控件不超出卡片) ---
    inline void PushClipRect(const ImVec2& pos, const ImVec2& size) {
        ImGui::GetWindowDrawList()->PushClipRect(pos, pos + size, true);
    }

    inline void PopClipRect() {
        ImGui::GetWindowDrawList()->PopClipRect();
    }
}
