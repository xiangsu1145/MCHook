#include "Module.h"

#include "../FeatureManager.h"

void Module::setEnabled(bool enabled) {
    bool prevEnabled = mEnabled;

    if (enabled == prevEnabled) return;

    auto holder = nes::make_holder<ModuleStateChangeEvent>(this, enabled, prevEnabled);
    gFeatureManager->mDispatcher->trigger(holder);
    if (holder->isCanceled()) return;

    mEnabled = enabled;
    
    if (mEnabled) {
        onEnable();
    }
    else {
        onDisable();
    }
}