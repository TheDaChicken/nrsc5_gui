//
// Created by TheDaChicken on 2/20/2025.
//

#include "Connection.h"

#include <cassert>

#include "SqliteHelper.h"

SQLite::Connection::~Connection()
{
	prepared_statements_.clear();
	sqlite3_close(db_);
}

SQLiteError SQLite::Connection::Open(
	const std::filesystem::path &path,
	const int flags)
{
	return static_cast<SQLiteError>(sqlite3_open_v2(path.string().data(), &db_, flags, nullptr));
}

tl::expected<SQLite::StatementHandle, SQLiteError> SQLite::Connection::Prepare(const std::string &sql)
{
	assert(db_ != nullptr);

	auto iter = prepared_statements_.find(sql);
	if (iter == prepared_statements_.end())
	{
		sqlite3_stmt *stmt = nullptr;

		int ret = sqlite3_prepare_v2(db_, sql.data(), sql.size(), &stmt, nullptr);
		if (ret != SQLITE_OK)
			return tl::unexpected(static_cast<SQLiteError>(ret));

		iter = prepared_statements_.emplace(sql,stmt).first;
	}

	return StatementHandle(iter->second);
}
