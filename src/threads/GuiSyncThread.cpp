//
// Created by TheDaChicken on 8/6/2024.
//

#include "GuiSyncThread.h"
#include "utils/Log.h"

constexpr int DEFAULT_TIMEOUT = 1300;
constexpr int DEFAULT_SLEEP = 3000;

GuiSyncThread::GuiSyncThread()
= default;

void GuiSyncThread::Stop()
{
	m_stop = true;
	m_queue.Close();

	if (isRunning())
		wait();
}

void GuiSyncThread::run()
{
	std::unique_ptr<QueueMessage> event;
	int sleep_time;

	while (!m_stop)
	{
		if (!m_queue.Pop(event, std::chrono::seconds{50}))
		{
			continue;
		}

		assert(event->GetMessageType() == QueueMessage::GENERAL_GUI_UPDATE);

		// Take ownership of the event
		auto guiMessage = std::shared_ptr<GuiSyncEvent>(dynamic_cast<GuiSyncEvent *>(event.release()));

		if (!m_stream)
		{
			Logger::Log(warn, "No Output Stream. Sending event");
			emit SyncEvent(guiMessage);
			continue;
		}

		const PaTime start_time = guiMessage->time_;
		int timeout = DEFAULT_TIMEOUT;

		// Wait for the event to happen with a timeout
		while ((sleep_time = SleepTime(start_time, m_stream->GetTime()) > 0 && timeout-- > 0 && !m_stop))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		}

		if (timeout <= 0)
			Logger::Log(warn, "GuiSyncThread: Timeout running syncEvent() time: {} now: {}", guiMessage->time_, m_stream->GetTime());

		emit SyncEvent(guiMessage);
	}
}

int GuiSyncThread::SleepTime(const PaTime &start_time, const PaTime &current_time)
{
	if (current_time > start_time)
		return 0;

	// convert to milliseconds & floor to the nearest millisecond
	int sleepTime = static_cast<int>((start_time - current_time) * 1000);

	// Limit the sleep time to 3 seconds to prevent the thread from hanging indefinitely.
	if (sleepTime > DEFAULT_SLEEP)
		sleepTime = DEFAULT_SLEEP;

	return sleepTime;
}
