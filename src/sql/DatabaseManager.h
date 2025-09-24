//
// Created by TheDaChicken on 8/27/2025.
//

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <filesystem>

#include "Table.h"

class DatabaseManager
{
	public:
		tl::expected<void, SQLiteError> Open(const std::filesystem::path &path, int pool_num);
		tl::expected<std::shared_ptr<SQLite::Connection>, SQLiteError> PopConnection();

	private:
		SQLite::ConnectionPool pool_;
};

#endif //DATABASEMANAGER_H
