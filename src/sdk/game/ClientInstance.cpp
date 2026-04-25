#include "ClientInstance.h"

#include "src/utils/MemoryUtils.h"

LocalPlayer* ClientInstance::getLocalPlayer() {
	auto call = (LocalPlayer*(__fastcall*)(ClientInstance*))(vtable[35]);
	return call(this);
}

Minecraft* ClientInstance::getMinecraft() {
	//ClientInstance::update调用了Minecraft::update，可以看到偏移
	return MemoryUtils::member_at<Minecraft*>(this, 0xE0);
}