#include "Main.h"

#include "string"

#include "include/Minhook.h"
#include "src/hook/ActorHook.h"
#include "src/hook/ClientInstanceHook.h"
#include "src/hook/WGLHook.h"
#include "src/utils/Logger.h"
#include "src/hook/PacketSendHook.h"
#include "src/features/FeatureManager.h"
#include <sdk/SigManager.h>

HMODULE Main::hModule = nullptr;
int64 Main::baseAddress = 0;

void Main::init(const HMODULE hModule) {
    baseAddress = (int64)GetModuleHandleA("Minecraft.Windows.exe");
	if (baseAddress == 0) {
        MessageBoxA(nullptr, "No baseAddress error", "MCHook", MB_OK);
		return;
    }

	Logger::info("baseAddress: %p", baseAddress);

	SigManager::init();

	gFeatureManager = std::make_shared<FeatureManager>();

	gFeatureManager->init();

	MH_Initialize();
	WGLHook::init();
	ClientInstanceHook::init();
	ActorHook::init();
	PacketSendHook::init();
	
}