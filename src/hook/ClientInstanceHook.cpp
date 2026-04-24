#include "ClientInstanceHook.h"

#include "src/sdk/game/ClientInstance.h"

#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"
#include <MinHook.h>

void __fastcall ClientInstanceHook::hookUpdate(ClientInstance* thiz, unsigned a2) {
	ClientInstance::instance = thiz;

	return originalUpdate(thiz, a2);
}

void ClientInstanceHook::init() {
    auto result = SigManager::get(SignatureID::ClientInstance_update);
    if (!result) {
        Logger::error("Failed to find ClientInstance::update");
    }
    void* target = (void*)(*result);

    MH_CreateHook(target, &hookUpdate, (void**)&originalUpdate);
    MH_EnableHook(target);
}