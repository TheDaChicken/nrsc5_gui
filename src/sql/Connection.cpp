//
// Created by TheDaChicken on 2/20/2025.
//

#include "Connection.h"
#include "SqliteHelper.h"

SQLite::Connection::~Connection()
{
	for (auto &stmt : prepared_statements_)
	{
		sqlite3_finalize(stmt.second);
	}
	sqlite3_close(db_);
}

UTILS::StatusCodes SQLite::Connection::Open(
	const std::filesystem::path &path,
	const int flags)
{
	int ret;

	SQLITE_RETURN_ERRSTR(sqlite3_open_v2(path.string().data(), &db_, flags, nullptr));

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes SQLite::Connection::Prepare(const std::string &sql, sqlite3_stmt *&stmt)
{
	int ret;

	if (prepared_statements_.find(sql) != prepared_statements_.end())
	{
		stmt = prepared_statements_[sql];
		return UTILS::StatusCodes::Ok;
	}

	SQLITE_RETURN_ERRSTR(sqlite3_prepare_v2(db_, sql.data(), sql.size(), &stmt, nullptr));

	prepared_statements_[sql] = stmt;
	return UTILS::StatusCodes::Ok;
}
