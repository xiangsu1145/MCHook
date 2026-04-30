#pragma once

#include "../Module.h"

class Fly : public ModuleBase<Fly>
{
public:
	enum class Mode {
		Motion,
		Position
	};
	EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the fly module", Mode::Motion, "Motion", "Position");
	NumberSetting mSpeed = NumberSetting("Speed", "The fly speed", 1.0f, 0.1f, 10.0f, 0.1f);
	BoolSetting mBypass = BoolSetting("Bypass", "Slight up/down motion to bypass anti-cheat", false);

	Fly() : ModuleBase("Fly", "Allows you to fly", ModuleCategory::Movement, 'G', false) {
		addSettings(&mMode, &mSpeed, &mBypass);
	}
	void onEnable() override;
	void onDisable() override;
	void onPacketOutEvent(PacketOutEvent& event);
};