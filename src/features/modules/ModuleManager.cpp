#include "ModuleManager.h"

#include "combat/KillAura.h"
#include "combat/InfiniteAura.h"
#include "movement/Speed.h"
#include "movement/Fly.h"
#include "player/NoFall.h"
#include "visual/ClickGui.h"
#include "visual/Notifications.h"
#include "misc/Disabler.h"
#include "src/utils/Logger.h"


void ModuleManager::init() {
	{//Combat
		mModules.emplace_back(std::make_shared<KillAura>());
		mModules.emplace_back(std::make_shared<InfiniteAura>());
	}
	{//Movement
		mModules.emplace_back(std::make_shared<Speed>());
		mModules.emplace_back(std::make_shared<Fly>());
	}
	{//Player
		mModules.emplace_back(std::make_shared<NoFall>());
	}
	{//Visual
		mModules.emplace_back(std::make_shared<ClickGui>());
		mModules.emplace_back(std::make_shared<Notifications>());
	}
	{//Misc
		mModules.emplace_back(std::make_shared<Disabler>());
	}
}

void ModuleManager::shutdown() {
	for (const auto& module : mModules) {
		if (module->isEnabled()) {
			module->onDisable();
			module->disable();
		}
	}
	mModules.clear();
}