#pragma once
#include "../Module.h"
#include <imgui/imgui.h>
#include "../../event/RenderEvent.h"
#include "../../event/KeyEvent.h"
#include "../../event/MouseEvent.h"

struct CategoryPanel {
    std::string name;
    float x, y;
    float width;
    bool dragging = false;
    float dragOffsetX = 0, dragOffsetY = 0;
    float animScroll = 0.f;
};

class ClickGui : public ModuleBase<ClickGui> {
public:
    ClickGui() : ModuleBase("ClickGui", "Open the ClickGui.", ModuleCategory::Visual, VK_RSHIFT, false) {

    }

    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(RenderEvent& event);
    void onKeyEvent(KeyEvent& event);
    void onMouseEvent(MouseEvent& event);


};