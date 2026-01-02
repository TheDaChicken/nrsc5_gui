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
	return static_cast<SQLiteError>(
		sqlite3_open_v2(
			path.string().c_str(),
			&db_,
			flags,
			nullptr));
}

tl::expected<SQLite::StatementHandle, SQLiteError> SQLite::Connection::Prepare(const std::string &sql)
{
	assert(db_ != nullptr);

	const auto iter = prepared_statements_.find(sql);
	if (iter != prepared_statements_.end())
	{
		return StatementHandle(iter->second);
	}

	auto stmt = std::make_shared<PreparedStatement>();

	int ret = stmt->Create(db_, sql);
	if (ret != SQLITE_OK)
		return tl::unexpected(static_cast<SQLiteError>(ret));

	prepared_statements_[sql] = stmt;

	return StatementHandle(std::move(stmt));
}
