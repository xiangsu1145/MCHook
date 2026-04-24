#include "ActorHook.h"
#include "MinHook.h"

#include "Main.h"
#include "src/utils/Logger.h"

#include "src/sdk/game/world/actor/Actor.h"

#include "src/features/FeatureManager.h"

#include "src/sdk/SigManager.h"
#include "src/utils/Logger.h"

void __fastcall ActorHook::hookedNormalTick(Actor* self) {//这个fastcall的this指针是第一个参数，所以self就是Actor实例
    auto holder = nes::make_holder<NormalTickEvent>((LocalPlayer*)self);

	gFeatureManager->mDispatcher->trigger(holder);

    originalNormalTick(self);
}

void ActorHook::init() {
    auto result = SigManager::get(SignatureID::Actor_normalTick);
    if (!result) {
        Logger::error("Failed to find Actor::normalTick");
    }
    void* target = (void*)(*result);

    MH_CreateHook(target, &hookedNormalTick, (void**)&originalNormalTick);
    MH_EnableHook(target);

    Logger::info("ActorHook init done, target: %p", target);
}