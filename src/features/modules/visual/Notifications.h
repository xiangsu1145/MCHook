#pragma once

#include "../Module.h"

#include "../../event/NotifyEvent.h"

class Notifications : public ModuleBase<Notifications> {
public:
	BoolSetting mLimitNotifications = BoolSetting("Limit notifications", "Limit the number of notifications shown at one time", false);
	NumberSetting mMaxNotifications = NumberSetting("Max notifications", "The maximum number of notifications shown at one time", 6, 1, 25, 1);

	Notifications() : ModuleBase("Notifications", "Shows notifications on module toggle and other events", ModuleCategory::Visual, 0, false) {
		addSettings(&mLimitNotifications, &mMaxNotifications);
	}
	virtual void onEnable() override;
	virtual void onDisable() override;
	void onRenderEvent(RenderEvent& event);
	void onModuleStateChangeEvent(ModuleStateChangeEvent& event);

	std::vector<Notification> mNotifications;


};

