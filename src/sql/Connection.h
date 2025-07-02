//
// Created by TheDaChicken on 2/20/2025.
//

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include <filesystem>

#include <map>

#include "Utils/Error.h"
#include "sqlite/sqlite3.h"

namespace SQLite
{
class Connection
{
	public:
		~Connection();

		UTILS::StatusCodes Open(const std::filesystem::path &path, int flags);
		UTILS::StatusCodes Prepare(const std::string& sql, sqlite3_stmt *&stmt);

		[[nodiscard]] sqlite3 *Get() const
		{
			return db_;
		}

	private:
		sqlite3 *db_{nullptr};
		std::map<std::string, sqlite3_stmt *> prepared_statements_;
};
} // namespace SQLite

#endif //SQLITECONNECTION_H
