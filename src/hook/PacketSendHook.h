#pragma once

class LoopbackPacketSender;
class Packet;

class PacketSendHook
{
public:
    typedef void(__fastcall* sendToServer_t)(LoopbackPacketSender* thiz, Packet* packet);
    static inline sendToServer_t originalFunc = nullptr;
    static void __fastcall hookSendToServer(LoopbackPacketSender* thiz, Packet *packet);

    static void init();
};

