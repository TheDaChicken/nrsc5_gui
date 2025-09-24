//
// Created by TheDaChicken on 2/16/2025.
//

#include "ConnectionPool.h"

#include "SqliteHelper.h"
#include "utils/Log.h"

constexpr int SQLITE_BUSY_TIME = 1000;

SQLite::ConnectionPool::ConnectionPool()
{
}

SQLite::ConnectionPool::~ConnectionPool()
{
	while (!idle_connections.empty())
		idle_connections.pop();

	condition.notify_all();
}

tl::expected<void, SQLiteError> SQLite::ConnectionPool::Open(
	const std::filesystem::path &path,
	const int pool_num)
{
	path_ = path;

	AllocateConnections(pool_num);
	return {};
}

void SQLite::ConnectionPool::AllocateConnections(const int num)
{
	const int min_connections = static_cast<int>(idle_connections.size());

	for (int i = min_connections; i < num; i++)
	{
		std::shared_ptr<Connection> &connection = idle_connections.emplace(std::make_shared<Connection>());
		CreateConnection(connection);
	}
}

tl::expected<std::shared_ptr<SQLite::Connection>, SQLiteError> SQLite::ConnectionPool::Pop()
{
	std::unique_lock lock(mutex);
	return PopConnection();
}

tl::expected<std::shared_ptr<SQLite::Connection>, SQLiteError> SQLite::ConnectionPool::WaitFor(
	const std::chrono::duration<int, std::milli> timeout)
{
	std::unique_lock lock(mutex);
	while (idle_connections.empty())
		condition.wait_for(lock, timeout);
	return PopConnection();
}

void SQLite::ConnectionPool::ReturnBack(const std::shared_ptr<Connection> &&connection)
{
	{
		std::unique_lock lock(mutex);
		idle_connections.push(connection);
	}
	condition.notify_all();
}

void SQLite::ConnectionPool::CreateConnection(const std::shared_ptr<Connection> &connection)
{
	if (const int ret = connection->Open(
			path_,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
			SQLITE_OPEN_NOMUTEX);
		ret != SQLITE_OK)
	{
		Logger::Log(err, "Failed to open database: {}", static_cast<int>(ret));
	}

	sqlite3_busy_timeout(connection->GetSq(), SQLITE_BUSY_TIME);
}

tl::expected<std::shared_ptr<SQLite::Connection>, SQLiteError> SQLite::ConnectionPool::PopConnection()
{
	if (idle_connections.empty())
		return tl::unexpected(Lite_Busy);

	const std::shared_ptr<Connection> connection = std::move(idle_connections.top());

	idle_connections.pop();

	return std::shared_ptr<Connection>(
		connection.get(),
		[this, connection](Connection*) {
			this->ReturnBack(std::move(connection));
		}
	);
}
