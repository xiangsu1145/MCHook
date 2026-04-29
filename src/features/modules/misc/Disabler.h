#pragma once

#include "../Module.h"

class Disabler : public ModuleBase<Disabler>
{
public:
	enum class Mode {
		Microsoft,
	};

	EnumSettingT<Mode> mMode = EnumSettingT<Mode>("Mode", "The mode of the disabler module", Mode::Microsoft, "Microsoft");

	Disabler() : ModuleBase("Disabler", "Disables various anti-cheat measures", ModuleCategory::Misc, 0, false) {
		addSettings(&mMode);
	}
	void onEnable() override;
	void onDisable() override;
	void onPacketOutEvent(PacketOutEvent& event);
};

