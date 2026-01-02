//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef SQLITEDATABASEMANAGER_H
#define SQLITEDATABASEMANAGER_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <stack>

#include "Connection.h"

namespace SQLite
{
class PooledConnection;

/**
* Manages many SqliteDatabase connections for thread-safe access
*/
class ConnectionPool
{
	public:
		explicit ConnectionPool();
		~ConnectionPool();

		tl::expected<void, SQLiteError> Open(const std::filesystem::path &path, int pool_num = 2);
		void AllocateConnections(int num);

		tl::expected<std::shared_ptr<Connection>, SQLiteError> Pop();
		tl::expected<std::shared_ptr<Connection>, SQLiteError> WaitFor(std::chrono::duration<int, std::milli> timeout);

		void ReturnBack(const std::shared_ptr<Connection> &&connection);
		int GetIdleCount()
		{
			std::lock_guard lock(mutex);
			return static_cast<int>(idle_connections.size());
		}

	private:
		tl::expected<std::shared_ptr<Connection>, SQLiteError> PopConnection();

		std::filesystem::path path_;
		std::stack<std::shared_ptr<Connection> > idle_connections;
		std::mutex mutex;
		std::condition_variable condition;
};

class PooledConnection
{
	public:
		PooledConnection(std::shared_ptr<Connection> conn, ConnectionPool *pool)
			: conn_(std::move(conn)), pool_(pool)
		{
			assert(conn_);
		}

		~PooledConnection()
		{
			if (conn_)
				pool_->ReturnBack(std::move(conn_));
		}

		PooledConnection(const PooledConnection &) = delete; // no copy
		PooledConnection &operator=(const PooledConnection &) = delete; // no copy-
		PooledConnection(PooledConnection &&) = default; // allow move construction
		PooledConnection &operator=(PooledConnection &&other) noexcept
		{
			if (this != &other)
			{
				if (conn_)
					pool_->ReturnBack(std::move(conn_));
				conn_ = std::move(other.conn_);
				pool_ = other.pool_;
			}
			return *this;
		}

		Connection *operator ->() const
		{
			return conn_.get();
		}

	private:
		std::shared_ptr<Connection> conn_;
		ConnectionPool *pool_;
};
}

#endif //SQLITEDATABASEMANAGER_H
