#pragma once

#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

enum class SignatureID {
    //Actor class

    Actor_getPosition,
    Actor_setPosition,
    Actor_getLevel,
    Actor_normalTick,
    Actor_getRegion,
    Actor_getDimension,
    Actor_getFallDistance,
    Actor_jumpFromGround,

    //Player class

    Player_getGameMode,
    Player_getSupplies,

    //LocalPlayer class

    LocalPlayer_swing,


    //ClientInstance

    ClientInstance_update,

    //Minecraft
    Minecraft_update,

    //LoopbackPacketSender
	LoopbackPacketSender_sendToServer,

    //MinecraftPackets
    MinecraftPackets_createPacket,

    //Json_FastWriter
    Json_FastWriter_write,
};

enum class SignatureType {
    Direct,
    Call,
    Lea,
    Offset
};

class SignatureInfo {
public:
    SignatureID mId;
    SignatureType mType;
    std::vector<std::string> mPatterns;
    int mOffset = 0;

    SignatureInfo(SignatureID id, SignatureType type, std::string pattern, int offset = 0)
        : mId(id), mType(type), mPatterns({ std::move(pattern) }), mOffset(offset) {}

    SignatureInfo(SignatureID id, SignatureType type, std::initializer_list<std::string> patterns, int offset = 0)
        : mId(id), mType(type), mPatterns(patterns), mOffset(offset) {}
};

class SigManager {
public:
    static void init();
    static std::optional<uintptr_t> get(SignatureID id);

};
