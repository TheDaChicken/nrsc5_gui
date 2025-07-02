//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef SQLITEDATABASEMANAGER_H
#define SQLITEDATABASEMANAGER_H

#include <QThreadStorage>

#include "sql/Connection.h"
#include "utils/Error.h"

namespace SQLite
{
/**
* Manages many SqliteDatabase connections for thread-safe access
*/
class ConnectionPool
{
	public:
		UTILS::StatusCodes Open(const std::filesystem::path &path);

		/**
		* @brief Get a connection to the database for the current thread
		* @return status of the connection
		*/
		Connection &GetConnection();

	private:
		std::filesystem::path path_;

		QThreadStorage<Connection> thread_connections_;
};
}

#endif //SQLITEDATABASEMANAGER_H
