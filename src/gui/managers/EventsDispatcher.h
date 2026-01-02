//
// Created by TheDaChicken on 8/5/2025.
//

#ifndef SCREENSYNCMANAGER_H
#define SCREENSYNCMANAGER_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

struct EventData
{
	virtual ~EventData() = default;

	std::chrono::steady_clock::time_point timestamp;
};

class EventsTimerDispatcher
{
	public:
		explicit EventsTimerDispatcher()
		{
		}

		EventsTimerDispatcher(EventsTimerDispatcher const &) = delete;
		EventsTimerDispatcher &operator=(EventsTimerDispatcher const &) = delete;

		[[nodiscard]] int GetBufferedFrames() const
		{
			std::scoped_lock lock(mutex_);
			return static_cast<int>(events_.size());
		}

		[[nodiscard]] bool HasEvents() const
		{
			std::scoped_lock lock(mutex_);
			return !events_.empty();
		}

		void PushFrame(std::unique_ptr<EventData> &&frame);

		std::chrono::seconds::rep Tick(const std::function<void(std::unique_ptr<EventData>)> &callback);

	private:
		std::queue<std::unique_ptr<EventData> > events_;
		mutable std::mutex mutex_;
};

#endif //SCREENSYNCMANAGER_H
