#pragma once

#include <cstdint>

class IPacketHandlerDispatcher {
public:
	uintptr_t** vtable;

public:
	void* getPacketHandler() {
		return reinterpret_cast<void*>(vtable[1]);
	}
};
