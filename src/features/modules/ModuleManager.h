#pragma once

#include <vector>
#include "Module.h"
#include <memory>

class ModuleManager{
public:
	std::vector<std::shared_ptr<Module>> mModules;

	void init();
	void shutdown();
	std::vector<std::shared_ptr<Module>>& getModuleList() {
		return mModules;
	}

	template<typename T>
	T* getModule() {
		const auto& typeID = typeid(T).name();
		for(const auto& module :mModules){
			if(module->getTypeID() == typeID){
				return static_cast<T*>(module.get());
			}
		}
	}
	
};

