#include "Level.h"

#include "Main.h"

std::vector<Actor*> Level::getRuntimeActorList() {
    std::vector<Actor*> result;
	auto call = (std::vector<Actor*> (__fastcall*)(void*,std::vector<Actor*>*)) (Main::baseAddress + 0x4335640);
	return call(this,&result);
}

void Level::forEachPlayer(std::function<bool(Player&)> callback) {
    auto call = (void(__fastcall*)(Level*, std::function<bool(Player&)>)) (Main::baseAddress + 0x4330040);
    call(this, callback);
}

std::vector<Player*> Level::getPlayers() {
    std::vector<Player*> players;
    reinterpret_cast<void(__fastcall*)(void*, std::function<bool(Player&)>)>(vtable[258])(
        this, [&](Player& actor) -> bool {
            players.push_back(&actor);
            return true;
        }
        );
    return players;
}