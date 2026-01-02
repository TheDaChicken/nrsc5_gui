//
// Created by TheDaChicken on 8/5/2025.
//

#include "EventsDispatcher.h"

void EventsTimerDispatcher::PushFrame(std::unique_ptr<EventData> &&frame)
{
	std::scoped_lock lock(mutex_);

	events_.push(std::move(frame));
}

std::chrono::seconds::rep EventsTimerDispatcher::Tick(
	const std::function<void(std::unique_ptr<EventData>)> &callback)
{
	while (true)
	{
		std::unique_ptr<EventData> frame;

		{
			std::unique_lock lock(mutex_);

			if (events_.empty())
				break;

			const auto stream_time = std::chrono::steady_clock::now();
			const auto event_time = events_.front()->timestamp;

			if (event_time > stream_time)
			{
				const auto diff = std::chrono::duration_cast<std::chrono::seconds>(stream_time - event_time).count();
				return std::min(diff, static_cast<long long>(10));
			}

			frame = std::move(events_.front());
			events_.pop();
		}

		if (frame)
			callback(std::move(frame));
	}

	return 0;
}

