#pragma once

#include "Event.h"

class Module;

class ModuleStateChangeEvent : public CancelableEvent {
public:
	Module* mModule;
	bool mEnabled;
	bool mWasEnabled;

	explicit ModuleStateChangeEvent(Module* module, bool enabled, bool wasEnabled) : CancelableEvent() {
		mModule = module;
		mEnabled = enabled;
		mWasEnabled = wasEnabled;
	}
};