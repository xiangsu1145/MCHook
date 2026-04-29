#pragma once

class Packet;

class LoopbackPacketSender
{
public:
	static inline LoopbackPacketSender* instance = nullptr;
	static LoopbackPacketSender* getSender() {
		return instance;
	};
	void sendToServer(Packet* packet);

};

