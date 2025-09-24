//
// Created by TheDaChicken on 8/3/2025.
//

#include "ThreadPool.h"

ThreadPool::~ThreadPool()
{
	Stop();
}

void ThreadPool::Start(uint32_t n)
{
	if (n == 0)
		n = std::thread::hardware_concurrency(); // Default to max # of threads the system supports

	for (uint32_t i = 0; i < n; ++i)
	{
		workers.emplace_back(&ThreadPool::ThreadLoop, this);
	}
}

void ThreadPool::Stop()
{
	{
		std::unique_lock lock(mutex_);
		stop = true;
	}
	condition_.notify_all();
	for (auto &thread : workers)
	{
		thread.join();
	}
	workers.clear();
}

void ThreadPool::QueueJob(const std::function<void()> &job)
{
	{
		std::unique_lock lock(mutex_);
		jobs.push(job);
	}
	condition_.notify_one();
}

bool ThreadPool::Busy()
{
	bool busy;
	{
		std::unique_lock lock(mutex_);
		busy = !jobs.empty();
	}
	return busy;
}

void ThreadPool::ThreadLoop()
{
	while (true)
	{
		std::function<void()> job;
		{
			std::unique_lock lock(mutex_);
			condition_.wait(lock,
			                [this]
			                {
				                return !jobs.empty() || stop;
			                });
			if (stop)
				break;

			job = jobs.front();
			jobs.pop();
		}
		job();
	}
}
