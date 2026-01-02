//
// Created by TheDaChicken on 8/3/2025.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "Expected.hpp"

namespace UTILS
{
template<typename T, typename F>
struct FutureState
{
	bool finished = false;
	tl::expected<T, F> value;

	std::mutex m;
	std::condition_variable cv;
};

template<typename T, typename F>
class Future
{
	public:
		Future() = default;

		explicit Future(std::shared_ptr<FutureState<T, F> > state)
			: state_(std::move(state))
		{
		}

		void Wait() const
		{
			if (!state_)
				return;

			std::unique_lock<std::mutex> lock(state_->m);
			state_->cv.wait(lock, [&] { return state_->finished; });
		}

		tl::expected<T, F> Get() const
		{
			if (!state_)
				throw std::invalid_argument("Invalid state");

			std::unique_lock<std::mutex> lock(state_->m);
			state_->cv.wait(lock, [&] { return state_->finished; });
			return std::move(state_->value);
		}

		[[nodiscard]] bool IsValid() const
		{
			return state_ != nullptr;
		}

		[[nodiscard]] bool IsReady() const
		{
			if (!state_)
				return false;

			std::unique_lock<std::mutex> lock(state_->m);
			return state_->finished;
		}

		void SetFinish()
		{
			if (!state_)
				return;

			std::unique_lock<std::mutex> lock(state_->m);
			state_->finished = true;
		}

		void Reset()
		{
			state_.reset();
		}

	private:
		std::shared_ptr<FutureState<T, F> > state_;
};

template<typename T, typename F = std::exception>
class Promise
{
	public:
		explicit Promise()
			: state_(std::make_shared<FutureState<T, F> >())
		{
		}

		void SetFinished()
		{
			{
				std::lock_guard lock(state_->m);
				state_->finished = true;
			}
			state_->cv.notify_all();
		}

		void SetValue(T &&value)
		{
			{
				std::lock_guard lock(state_->m);
				state_->value = std::move(value);
				state_->finished = true;
			}
			state_->cv.notify_all();
		}

		void SetValue(const T &value)
		{
			{
				std::lock_guard lock(state_->m);
				state_->value = value;
				state_->finished = true;
			}
			state_->cv.notify_all();
		}

		void SetFail(const F &value)
		{
			{
				std::lock_guard lock(state_->m);
				state_->value = tl::unexpected(value);
				state_->finished = true;
			}
			state_->cv.notify_all();
		}

		bool IsFinished() const
		{
			std::lock_guard lock(state_->m);
			return state_->finished;
		}

		Future<T, F> GetFuture()
		{
			return Future(state_);
		}

	private:
		std::shared_ptr<FutureState<T, F> > state_;
};

template<typename F>
class Promise<void, F>
{
	public:
		explicit Promise()
			: state_(std::make_shared<FutureState<void, F> >())
		{
		}

		Future<void, F> GetFuture()
		{
			return Future(state_);
		}

		void SetValue()
		{
			{
				std::lock_guard lock(state_->m);
				state_->finished = true;
			}
			state_->cv.notify_all();
		}

		bool IsFinished()
		{
			std::lock_guard lock(state_->m);
			return state_->finished;
		}

	private:
		std::shared_ptr<FutureState<void, F> > state_;
};

class ThreadPool
{
	public:
		static ThreadPool &GetInstance()
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
}

#endif //THREADPOOL_H
