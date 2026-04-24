#pragma once

#include "Event.h"

class MouseEvent : public CancelableEvent {
public:
    int mButton; // 0=左键 1=右键 2=中键
    bool mPressed;
    float mX;
    float mY;
    explicit MouseEvent(int button, bool pressed,float x,float y) : mButton(button), mPressed(pressed),mX(x),mY(y), CancelableEvent() {}
};