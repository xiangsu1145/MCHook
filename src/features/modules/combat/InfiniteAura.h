#pragma once

#include "../Module.h"

class InfiniteAura : public ModuleBase<InfiniteAura>
{
public:
	enum class Mode {
		Normal
	};

	EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the InfiniteAura module", Mode::Normal, "Normal");
	NumberSetting mRange = NumberSetting("Range", "The range of the InfiniteAura", 35.f, 1.f, 200.f, 0.1f);
	NumberSetting mAPS = NumberSetting("APS", "Attacks per second", 10.f, 1.f, 40.f, 0.1f);

	InfiniteAura() : ModuleBase("InfiniteAura", "Gives you infinite reach and attack speed", ModuleCategory::Combat, 0, false) {
		addSettings(&mMode, &mRange, &mAPS);
	}
	void onEnable() override;
	void onDisable() override;
	void onTickEvent(NormalTickEvent& event);
};

