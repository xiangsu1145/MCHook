#pragma once
#include "../Module.h"

class KillAura : public ModuleBase<KillAura>{
public:
	NumberSetting mRange = NumberSetting("Range", "The range of the KillAura", 4.1f, 3.f, 7.f, 0.01f);
	RangeSetting mAPS = RangeSetting("APS", "Attacks per second", 8.f, 12.f, 1.f, 20.f, 0.01f);
	KillAura() : ModuleBase("KillAura","Automatically attacks nearby enemies",ModuleCategory::Combat,'R', false) {
		addSettings(&mRange, &mAPS);
	}
	void onEnable() override;
	void onDisable() override;
	void onTickEvent(NormalTickEvent& event);
};

