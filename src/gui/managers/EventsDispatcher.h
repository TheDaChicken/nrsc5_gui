//
// Created by TheDaChicken on 8/5/2025.
//

#ifndef SCREENSYNCMANAGER_H
#define SCREENSYNCMANAGER_H

#include <mutex>
#include <queue>

#include "RadioChannel.h"

enum FrameTypes
{
	UNKNOWN,
	FREQUENCY_CHANGE,
	PROGRAM_CHANGE,
	STATION_NAME,
	STATION_ID,
	STATION_PROGRAM,
	STATION_ID3,
	STATION_LOT,
	AUDIO_FRAME
};

struct EventData
{
	virtual ~EventData() = default;

	explicit EventData(const FrameTypes type_) : type(type_)
	{
	}

	FrameTypes type = UNKNOWN;
	std::chrono::steady_clock::time_point timestamp;
};

class EventsDispatcher
{
	public:
		explicit EventsDispatcher()
		{
		}

		EventsDispatcher(EventsDispatcher const &) = delete;
		EventsDispatcher &operator=(EventsDispatcher const &) = delete;

		[[nodiscard]] int GetBufferedFrames() const
		{
			std::lock_guard lock(mutex_);
			return static_cast<int>(events_.size());
		}

		[[nodiscard]] bool HasEvents() const
		{
			std::lock_guard lock(mutex_);
			return !events_.empty();
		}

		void PushFrame(std::unique_ptr<EventData> &&frame);

		std::chrono::seconds::rep Tick(const std::function<void(std::unique_ptr<EventData>)> &callback);

	private:
		mutable std::mutex mutex_;
		std::queue<std::unique_ptr<EventData> > events_;
};

#endif //SCREENSYNCMANAGER_H
