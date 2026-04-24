#include "ActorUtils.h"

#include "src/sdk/game/world/actor/Actor.h"
#include "src/sdk/game/world/actor/mob/player/LocalPlayer.h"
#include "src/sdk/game/world/actor/components/ActorOwnerComponent.hpp"
#include "src/sdk/game/world/actor/components/ActorTypeComponent.hpp"
#include "src/utils/Logger.h"

std::vector<Actor*> ActorUtils::getActorList(ActorType filterType) {
    //Actor* local = Actor::getPrimaryLocalPlayer();
    //if (!local) return {};

    //std::vector<Actor*> actors;
    //auto& registry = local->getEntityContext().mEnTTRegistry;

    //for (auto&& [entId, owner, actorType] : registry.view<ActorOwnerComponent, ActorTypeComponent>().each()) {
    //    if (!registry.valid(entId)) continue;
    //    if (!owner.mActor) continue;
    //    if (owner.mActor == local) continue;

    //    // 过滤type，None表示不过滤
    //    if (filterType != ActorType::None) {
    //        if (((int)actorType.mType & (int)filterType) != (int)filterType) continue;
    //    }

    //    actors.push_back(owner.mActor);
    //}

    //return actors;
    return {};
}