#pragma once

#include "Event.h"
#include <string>

class Notification {
public:
	enum class Type {
		Info,
		Warning,
		Error
	};
	std::string mMessage;
	Type mType;
	float mDuration;
	float mCurrentDuration = 0.0f;
	float mTimeShown = 0.0f;
	bool mIsTimeUp = false;

	float getPercentComplete() const {
		return mTimeShown / mDuration;
	}

	Notification(std::string message, const Type type, const float duration) : mMessage(std::move(message)), mType(type), mDuration(duration), mCurrentDuration(0.f) {}

};

class NotifyEvent : public Event
{
public:
	Notification mNotification;
	explicit NotifyEvent(Notification notification) : mNotification(std::move(notification)) {}
};

