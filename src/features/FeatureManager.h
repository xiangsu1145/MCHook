#pragma once

#include "modules/ModuleManager.h"

#include <nes/event_dispatcher.hpp>

class FeatureManager
{
public:
	std::unique_ptr<nes::event_dispatcher> mDispatcher;
	std::shared_ptr<ModuleManager> mModuleManager = nullptr;
	
	void init();
	void shutdown();
};

inline std::shared_ptr<FeatureManager> gFeatureManager = nullptr;
