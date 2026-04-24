#include "FeatureManager.h"

void FeatureManager::init() {
	mDispatcher = std::make_unique<nes::event_dispatcher>();

	mModuleManager = std::make_shared<ModuleManager>();
	mModuleManager->init();

}

void FeatureManager::shutdown() {
	mModuleManager->shutdown();
}	