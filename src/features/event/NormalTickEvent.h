#pragma once

#include "Event.h"

class LocalPlayer;

class NormalTickEvent : public Event {
public:
	LocalPlayer* mActor;

	explicit NormalTickEvent(LocalPlayer* actor) : mActor(actor) {

	}
};