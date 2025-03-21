//
// Created by TheDaChicken on 2/16/2025.
//

#include "ConnectionPool.h"

#include <QThread>

#include "SqliteHelper.h"

enum Settings : int
{
	SQLITE_BUSY_TIME = 1000
};

UTILS::StatusCodes SQLite::ConnectionPool::Open(const std::filesystem::path &path)
{
	path_ = path;
	return UTILS::StatusCodes::Ok;
}

SQLite::Connection &SQLite::ConnectionPool::GetConnection()
{
	Connection &connection = thread_connections_.localData();

	if (!connection.Get())
	{
		Logger::Log(debug, "SQLiteConnectionPool: Opening database: {} for {}", path_.string(), QThread::currentThreadId());

		if (const UTILS::StatusCodes ret = connection.Open(path_,
		                                                   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
		                                                   SQLITE_OPEN_NOMUTEX);
			ret != UTILS::StatusCodes::Ok)
		{
			Logger::Log(err, "Failed to open database: {}", static_cast<int>(ret));
		}

		sqlite3_busy_timeout(connection.Get(), SQLITE_BUSY_TIME);
	}

	return connection;
}
