//
// Created by TheDaChicken on 11/22/2025.
//

#ifndef NRSC5_GUI_PACKETQUEUE_H
#define NRSC5_GUI_PACKETQUEUE_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

enum QueueStatus
{
	QUEUE_FULL,
	QUEUE_GOT,
	QUEUE_EMPTY,
	QUEUE_STOPPED,
};

template<class T>
class ThreadFreeQueue
{
	public:
		explicit ThreadFreeQueue(const size_t max_size = 100)
			: running_(false), max_size_(max_size)
		{
		}

		~ThreadFreeQueue()
		{
			Clear();
		}

		void Start()
		{
			{
				std::lock_guard lock(mutex_);
				running_ = true;
			}
		}

		void Stop()
		{
			{
				std::lock_guard lock(mutex_);
				running_ = false;
			}
			cond_var_.notify_all();
		}

		QueueStatus Add(T &&queue)
		{
			std::lock_guard lock(mutex_);
			if (!running_)
				return QUEUE_STOPPED;

			if (max_size_ > 0)
				return QUEUE_FULL;

			queue_.push(std::move(queue));
			cond_var_.notify_all();
			return QUEUE_GOT;
		}

		void Clear()
		{
			std::unique_lock lock(mutex_);
			while (!queue_.empty())
				queue_.pop();
		}

		int GetBlocking(T &message)
		{
			std::unique_lock lock(mutex_);

			while (queue_.empty())
			{
				cond_var_.wait(lock);

				if (!running_)
					return QUEUE_STOPPED;
			}

			message = std::move(queue_.front());
			queue_.pop();
			return QUEUE_GOT;
		}

		int Remove()
		{
			std::unique_lock lock(mutex_);
			if (!running_)
				return QUEUE_STOPPED;

			queue_.pop();
			return QUEUE_GOT;
		}

		bool IsFull()
		{
			return queue_.size() == max_size_;
		}

		bool IsOpen() const
		{
			return running_;
		}

	private:
		bool running_;
		std::mutex mutex_;
		std::condition_variable cond_var_;
		std::queue<T> queue_;
		std::size_t max_size_;
};

#endif //NRSC5_GUI_PACKETQUEUE_H
