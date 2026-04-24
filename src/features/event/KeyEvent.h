#pragma once

#include "Event.h"

class KeyEvent : public CancelableEvent {
public:
	int mKey;
	bool mPressed;
	explicit KeyEvent(int key, bool pressed) : mKey(key),mPressed(pressed),CancelableEvent(){

	}
};