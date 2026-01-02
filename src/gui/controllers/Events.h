//
// Created by TheDaChicken on 11/9/2025.
//

#ifndef NRSC5_GUI_EVENTS_H
#define NRSC5_GUI_EVENTS_H

#include "gui/managers/ImageManager.h"
#include "gui/managers/EventsDispatcher.h"
#include "nrsc5/Station.h"

#include <string>

using SessionId = uint32_t;

namespace UTILS
{
enum SessionStatus
{
	SESSION_CREATED,
	DELETED
};

template<typename EventType>
class EventBus
{
	public:
		void Subscribe(std::function<void(const EventType &)> callback)
		{
			std::scoped_lock lock(mutex);

			listeners.push_back(callback);
		}

		void Publish(const EventType &event)
		{
			std::scoped_lock lock(mutex);

			for (auto listener : listeners)
			{
				listener(event);
			}
		}

	private:
		// Group related events together
		std::list<std::function<void(const EventType&)>> listeners;
		std::mutex mutex;
};

} // namespace UTILS

struct HybridStateUi
{
	unsigned int program_id{0};

	std::string frequency_text;
	std::string formatted_name;

	GUI::TextureHandle station_logo_;
	GUI::TextureHandle primary_logo_;

	NRSC5::ID3 id3_;
};

struct HybridLotFrame final : EventData
{
	explicit HybridLotFrame(
		const std::shared_ptr<NRSC5::Lot> &event_)
		: lot(event_)
	{
	}

	std::shared_ptr<NRSC5::Lot> lot;
};

#endif //NRSC5_GUI_EVENTS_H
