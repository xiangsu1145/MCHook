#pragma once

#include "../imgui/imgui.h"

#pragma once
#include <vector>

enum class Orientation {
    Horizontal,
    Vertical
};

class View {
public:
    ImVec2 measuredSize;   // 测量后得到的尺寸
    ImVec2 finalPos;       // 布局后确定的左上角位置

    virtual ~View() = default;

    // 测量：根据父容器可用尺寸计算自身期望尺寸
    virtual void measure(float parentWidth, float parentHeight) {
        // 默认实现：子类必须重写
        measuredSize = ImVec2(0, 0);
    }

    // 布局：父容器通知最终位置和大小
    virtual void layout(float left, float top, float right, float bottom) {
        finalPos = ImVec2(left, top);
        measuredSize = ImVec2(right - left, bottom - top);
    }

    // 绘制：在 finalPos 处绘制控件
    virtual void draw() = 0;
};

// 线性布局：自动排列子视图
class LinearLayout : public View {
public:
    Orientation orientation;
    float spacing;                     // 子视图之间的间距
    ImU32 backgroundColor;             // 背景颜色（例如 IM_COL32(50,50,60,255)）
    float cornerRadius;                // 圆角半径（贝塞尔曲线平滑圆角）

    std::vector<View*> children;

    LinearLayout(Orientation orient = Orientation::Vertical,
        float spacing = 8.0f,
        ImU32 bgColor = IM_COL32(0, 0, 0, 0),   // 默认透明
        float radius = 0.0f)
        : orientation(orient), spacing(spacing), backgroundColor(bgColor), cornerRadius(radius) {}

    // 添加子视图
    void addView(View* child) {
        children.push_back(child);
    }

    // 测量：计算所有子视图的期望尺寸，并确定自身尺寸
    void measure(float parentWidth, float parentHeight) override {
        // 先测量所有子视图
        for (auto child : children) {
            child->measure(parentWidth, parentHeight);
        }

        float totalWidth = 0, totalHeight = 0;
        if (orientation == Orientation::Horizontal) {
            for (auto child : children) {
                totalWidth += child->measuredSize.x;
                totalHeight = (std::max)(totalHeight, child->measuredSize.y);
            }
            if (children.size() > 1)
                totalWidth += spacing * (children.size() - 1);
        }
        else { // Vertical
            for (auto child : children) {
                totalWidth = (std::max)(totalWidth, child->measuredSize.x);
                totalHeight += child->measuredSize.y;
            }
            if (children.size() > 1)
                totalHeight += spacing * (children.size() - 1);
        }

        measuredSize = ImVec2(totalWidth, totalHeight);
    }

    // 布局：为每个子视图计算最终位置
    void layout(float left, float top, float right, float bottom) override {
        View::layout(left, top, right, bottom);

        float curX = left, curY = top;
        for (auto child : children) {
            float childW = child->measuredSize.x;
            float childH = child->measuredSize.y;

            if (orientation == Orientation::Horizontal) {
                child->layout(curX, top, curX + childW, top + childH);
                curX += childW + spacing;
            }
            else {
                child->layout(left, curY, left + childW, curY + childH);
                curY += childH + spacing;
            }
        }
    }

    // 绘制：先绘制背景（圆角矩形），再绘制子视图
    void draw() override {
        ImDrawList* draw = ImGui::GetWindowDrawList();

        // 绘制背景（如果颜色非透明且圆角半径 >= 0）
        if ((backgroundColor & IM_COL32_A_MASK) != 0 && cornerRadius > 0.0f) {
            draw->AddRectFilled(finalPos,
                ImVec2(finalPos.x + measuredSize.x, finalPos.y + measuredSize.y),
                backgroundColor,
                cornerRadius);
        }
        else if ((backgroundColor & IM_COL32_A_MASK) != 0) {
            // 无圆角时直接填充矩形
            draw->AddRectFilled(finalPos,
                ImVec2(finalPos.x + measuredSize.x, finalPos.y + measuredSize.y),
                backgroundColor);
        }

        // 绘制所有子视图
        for (auto child : children) {
            ImGui::SetCursorScreenPos(child->finalPos);
            child->draw();
        }
    }
};

class Text : public View {
public:
    std::string text;
    ImU32 color;

    Text(const std::string& str, ImU32 col = IM_COL32(255, 255, 255, 255))
        : text(str), color(col) {}

    void measure(float /*parentWidth*/, float /*parentHeight*/) override {
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        measuredSize = textSize;
    }

    void draw() override {
        ImGui::SetCursorScreenPos(finalPos);
        ImGui::GetWindowDrawList()->AddText(finalPos, color, text.c_str());
    }
};