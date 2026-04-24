#pragma once
#include "Event.h"

#include "src/sdk/game/network/packet/Packet.h"

class PacketOutEvent : public CancelableEvent
{
public:
	Packet* mPacket;
	
	explicit PacketOutEvent(Packet* packet) : CancelableEvent() {
		mPacket = packet;
	}

	template<typename T>
	T* getPacket() {
		return static_cast<T*>(mPacket);
	}
};