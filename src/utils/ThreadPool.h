//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "sql/connection/ConnectionPool.h"

class ThreadPool
{
	public:
		static ThreadPool& GetInstance()
		{
			static ThreadPool instance;
			return instance;
		}

		~ThreadPool();

		void Start(uint32_t n = 0);
		void Stop();

		void QueueJob(const std::function<void()> &job);
		bool Busy();

	private:
		void ThreadLoop();

		bool stop = false;
		std::mutex mutex_;
		std::condition_variable condition_;

		std::vector<std::thread> workers;
		std::queue<std::function<void()> > jobs;
};

#endif //THREADPOOL_H
