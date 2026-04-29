#pragma once

#include "../Module.h"

class Speed : public ModuleBase<Speed>
{
public:
	enum class Mode {
		Normal,
		Bhop,
		Lowhop
	};
	EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the speed module", Mode::Normal, "Normal", "Bhop", "Lowhop");
	NumberSetting mSpeed = NumberSetting("Speed", "The speed multiplier", 1.0f, 0.1f, 5.0f, 0.1f);
	Speed() : ModuleBase("Speed", "Increases your movement speed", ModuleCategory::Movement, 0, false) {
		addSettings(&mMode, &mSpeed);
	}
	void onEnable() override;
	void onDisable() override;
	void onPacketOutEvent(PacketOutEvent& event);

};

