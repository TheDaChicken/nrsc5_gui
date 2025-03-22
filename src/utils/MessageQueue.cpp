//
// Created by TheDaChicken on 8/6/2024.
//

#include "MessageQueue.h"
#include "Log.h"

MessageQueue::MessageQueue(int MAX_QUEUE_SIZE) : queueSize_(MAX_QUEUE_SIZE)
{
}

MessageQueue::~MessageQueue() = default;

void MessageQueue::Close()
{
	std::lock_guard lock(mutex_);
	queue_.clear();
	event_.notify_all();
}

void MessageQueue::Flush(QueueMessage::MessageType type)
{
	queue_.remove_if([type](const std::unique_ptr<QueueMessage> &msg)
	{
		return type == QueueMessage::NONE || msg->IsType(type);
	});
}

std::optional<std::reference_wrapper<std::unique_ptr<QueueMessage> > >
MessageQueue::Find(const QString &id)
{
	auto it = std::find_if(queue_.begin(),
	                       queue_.end(),
	                       [&id](const std::unique_ptr<QueueMessage> &msg)
	                       {
		                       return msg->GetId() == id;
	                       });
	if (it != queue_.end())
		return {*it};
	return {};
}

int MessageQueue::Push(std::unique_ptr<QueueMessage> &&msg)
{
	if (!msg)
		return -1;

	std::lock_guard lock(mutex_);

	if (queue_.size() >= queueSize_)
	{
		return -2;
	}

	queue_.emplace_back(std::move(msg));
	event_.notify_all();

	return 0;
}

bool MessageQueue::Pop(std::unique_ptr<QueueMessage> &msg, const std::chrono::milliseconds &timeout)
{
	std::unique_lock lock(mutex_);

	if (queue_.empty())
	{
		if (timeout == std::chrono::milliseconds::zero())
		{
			return false;
		}

		std::cv_status ret = event_.wait_for(lock, timeout);
		if (ret == std::cv_status::timeout) return false;
	}

	if (queue_.empty())
	{
		return false;
	}

	auto &item(queue_.front());

	msg = std::move(item);

	queue_.pop_front();
	return true;
}
