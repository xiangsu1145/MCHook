#pragma once

#include "src/sdk/game/world/actor/Actor.h"
#include "../event/NormalTickEvent.h"
#include "../event/ModuleStateChangeEvent.h"
#include "../event/PacketOutEvent.h"
#include "../event/KeyEvent.h"
#include "../event/MouseEvent.h"
#include "../event/RenderEvent.h"
#include <string>
#include "Setting.h"
#include <nes/event_dispatcher.hpp>
#include <windows.h>

enum class ModuleCategory {
	Combat,
	Movement,
	Player,
	Visual,
	Misc
};

class Module{
public:
	std::string mName = "";
	std::string mDescription = "";
	ModuleCategory mCategory;
	bool mEnabled = false;
	int mKey;
	float mExpandAnim = 0.f;
public:
	void toggle() {
		setEnabled(!mEnabled);
	}

	void setEnabled(bool enabled);//bababoy

	bool isEnabled() {
		return mEnabled;
	}
	void enable() {
		setEnabled(true);
	}
	void disable() {
		setEnabled(false);
	}
	int getKey() {
		return mKey;
	}
	ModuleCategory getCategory() {
		return mCategory;
	}
	std::string getModuleName() {
		return mName;
	}
	Module(std::string name,std::string description,ModuleCategory category,int key,bool enabled){
		mName = std::move(name);
		mDescription = std::move(description);
		mCategory = category;
		mKey = key;
		//mEnabled = enabled;
		setEnabled(enabled);
	};

	BoolSetting mVisibleInArrayList = BoolSetting("Visible", "Whether the module is visible in the Arraylist", true);

	std::vector<class Setting*> mSettings = {
		&mVisibleInArrayList
	};
	void addSetting(Setting* setting) {
		mSettings.push_back(setting);
	}
	template <typename... Args>
	void addSettings(Args... args) {
		(addSetting(args), ...);
	}


	virtual void onDisable() {
	}
	virtual void onEnable() {
	}

};

template<typename T>
class ModuleBase : public Module {
public:
	ModuleBase(std::string name, std::string description, ModuleCategory category, int key, bool enabled)
		: Module(std::move(name), std::move(description), category, key, enabled) {

	}

	static std::string TypeID() {
		return typeid(T).name();
	}

	std::string getTypeID() {
		return TypeID();
	}
};