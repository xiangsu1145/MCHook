#pragma once

#include "../Module.h"

class NoFall : public ModuleBase<NoFall>
{
public:
	enum class Mode {
		Gliding,
		NoAuthPacket,
		Blink
	};
	NumberSetting mFallDistanceMin = NumberSetting("Min Fall Distance", "Start nofall when fall distance >= this value", 2.5f, 0.0f, 5.0f, 0.01f);
	NumberSetting mFallDistanceMax = NumberSetting("Max Fall Distance", "Disable nofall when fall distance > this value", 30.0f, 5.0f, 300.0f, 0.01f);
	EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "NoFall mode", Mode::Gliding, {
		"Gliding",
		"NoAuthPacket",
		"Blink"
		});
	NoFall() : ModuleBase("NoFall", "Prevents fall damage", ModuleCategory::Player, 'N', false)
	{
		addSettings(&mMode, &mFallDistanceMin, &mFallDistanceMax);
	}
	void onEnable() override;
	void onDisable() override;

	void onPacketOutEvent(PacketOutEvent& event);
};

