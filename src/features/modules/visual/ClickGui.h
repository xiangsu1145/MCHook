#pragma once
#include "../Module.h"
#include <imgui/imgui.h>
#include "../../event/RenderEvent.h"
#include "../../event/KeyEvent.h"
#include "../../event/MouseEvent.h"

class ClickGui : public ModuleBase<ClickGui> {
public:
    int mLastFrame = -1;

    ClickGui() : ModuleBase("ClickGui", "Open the ClickGui.", ModuleCategory::Visual, VK_RSHIFT, false) {
    }

    void onEnable() override;
    void onDisable() override;

    void onRenderEvent(RenderEvent& event);
    void onKeyEvent(KeyEvent& event);
    void onMouseEvent(MouseEvent& event);
};