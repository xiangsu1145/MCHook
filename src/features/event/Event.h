#pragma once

class Event {
public:
	explicit Event() = default;
};

class CancelableEvent : public Event {
public:
	bool mCanceled = false;
	void cancel() {
		mCanceled = true;
	}

	[[nodiscard]] bool isCanceled() const {
		return mCanceled;
	}
	void setCanceled(bool canceled) {
		mCanceled = canceled;
	}

	explicit CancelableEvent(bool cancelled = false) : mCanceled(cancelled) {
	}
};