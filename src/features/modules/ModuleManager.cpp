#include "ModuleManager.h"

#include "combat/KillAura.h"
#include "player/NoFall.h"
#include "visual/ClickGui.h"
#include "visual/Notifications.h"
#include "src/utils/Logger.h"


void ModuleManager::init() {
	{//Combat
		mModules.emplace_back(std::make_shared<KillAura>());
	}
	{//Movement
		
	}
	{//Player
		mModules.emplace_back(std::make_shared<NoFall>());
	}
	{//Visual
		Logger::info("ModuleManager Visual init");
		mModules.emplace_back(std::make_shared<ClickGui>());
		mModules.emplace_back(std::make_shared<Notifications>());
	}
	{//Misc

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