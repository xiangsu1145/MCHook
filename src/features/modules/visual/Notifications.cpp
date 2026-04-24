#include "Notifications.h"

#include "../../FeatureManager.h"
#include <include/imgui/imgui.h>
#include "../../event/NotifyEvent.h"
#include "src/utils/MathUtils.h"
#include "src/utils/Logger.h"

void Notifications::onEnable() {
    Logger::info("notificaiton setenable");
	gFeatureManager->mDispatcher->listen<RenderEvent, &Notifications::onRenderEvent>(this);
	gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Notifications::onModuleStateChangeEvent>(this);
}

void Notifications::onDisable() {
	gFeatureManager->mDispatcher->deafen<RenderEvent, &Notifications::onRenderEvent>(this);
	gFeatureManager->mDispatcher->deafen<ModuleStateChangeEvent, &Notifications::onModuleStateChangeEvent>(this);
}

static bool CalcSize(ImVec2& boxSize, float& yOff, float& x, ImVec2 screenSize, Notification* notification) {
    float beginX = screenSize.x - boxSize.x - 10.f;
    float endX = screenSize.x + boxSize.x;

    x = MathUtils::lerp(endX, beginX, notification->mCurrentDuration);
    yOff = MathUtils::lerp(yOff, yOff - boxSize.y, notification->mCurrentDuration);

    if (x > screenSize.x + boxSize.x && yOff > screenSize.y + boxSize.y) return true;

    return false;
}

void Notifications::onRenderEvent(RenderEvent& event)
{
    Logger::info("notification testttttt");
    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    float y = displaySize.y - 10.0f;
    float x;
    float delta = ImGui::GetIO().DeltaTime;

    // 移除过期的通知
    std::erase_if(mNotifications, [](const Notification& notification) {
        return notification.mIsTimeUp && notification.mTimeShown > notification.mDuration + 3.0f;
        });

    int drawnCount = 0;
    // 从最新的通知开始绘制（反向迭代）
    for (auto it = mNotifications.rbegin(); it != mNotifications.rend(); ++it) {
        auto& notification = *it;

        // If limiting is enabled and we've drawn enough, stop
        if (this->mLimitNotifications.mValue && drawnCount >= this->mMaxNotifications.mValue)
            break;

        // 更新通知状态
        notification.mTimeShown += delta;
        notification.mIsTimeUp = notification.mTimeShown >= notification.mDuration;
        notification.mCurrentDuration = MathUtils::lerp(
            notification.mCurrentDuration,
            notification.mIsTimeUp ? 0.0f : 1.0f,
            delta * 5.0f
        );

        float percentDone = MathUtils::clamp(notification.mTimeShown / notification.mDuration, 0.0f, 1.0f);

        // 计算大小
        constexpr float fontSize = 20.0f;
        const auto textSize = ImGui::GetFont()->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, notification.mMessage.c_str());
        auto boxSize = ImVec2(fmax(200.0f, 50 + textSize.x), textSize.y + 30.0f);

        // 定位 (假设 CalcSize 是你已有的函数)
        if (CalcSize(boxSize, y, x, displaySize, &notification))
            continue;

        // 根据类型设置颜色
        ImU32 bgColor = IM_COL32(50, 150, 255, 200); // Info: 蓝色
        if (notification.mType == Notification::Type::Warning)
            bgColor = IM_COL32(255, 200, 0, 200);     // Warning: 橙色
        else if (notification.mType == Notification::Type::Error)
            bgColor = IM_COL32(255, 50, 50, 200);      // Error: 红色

        // 应用 mCurrentDuration 动画到透明度
        uint8_t alpha = static_cast<uint8_t>(200 * notification.mCurrentDuration);
        bgColor = (bgColor & 0x00FFFFFF) | (static_cast<ImU32>(alpha) << 24);

        // --- 绘制动画矩形 ---
        // 计算当前动画高度
        float currentHeight = boxSize.y * notification.mCurrentDuration;
        ImVec2 rectMin(x, y - (boxSize.y - currentHeight));
        ImVec2 rectMax(x + boxSize.x, y + currentHeight);

        // 绘制背景
        drawList->AddRectFilled(rectMin, rectMax, bgColor, 5.0f);

        // 绘制进度条 (可选，根据 percentDone)
        ImVec2 progressMax(x + (boxSize.x * percentDone), rectMax.y);
        drawList->AddRectFilled(rectMin, progressMax, IM_COL32(255, 255, 255, static_cast<int>(50 * notification.mCurrentDuration)), 5.0f);

        // 绘制文本
        float textAlpha = 255 * notification.mCurrentDuration;
        drawList->AddText(
            ImGui::GetFont(), fontSize,
            ImVec2(x + 10, rectMin.y + (currentHeight - textSize.y) * 0.5f),
            IM_COL32(255, 255, 255, static_cast<int>(textAlpha)),
            notification.mMessage.c_str()
        );

        // 为下一个通知更新位置
        y -= (boxSize.y + 5.0f);
        drawnCount++;
    }
}

void Notifications::onModuleStateChangeEvent(ModuleStateChangeEvent& event) {
    Logger::info("Notifications::onModuleStateChangeEvent");
    if (event.isCanceled()) return;
	auto notification = Notification(event.mModule->getModuleName() + " was" + (event.mEnabled ? " enabled" : " disabled"), Notification::Type::Info, 3.f);
	mNotifications.push_back(notification);
}
