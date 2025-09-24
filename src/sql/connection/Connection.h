//
// Created by TheDaChicken on 2/20/2025.
//

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include <filesystem>

#include <unordered_map>

#include "PreparedStatement.h"
#include "SqliteHelper.h"
#include "../sqlite/sqlite3.h"
#include "utils/Expected.hpp"
#include "utils/Log.h"

namespace SQLite
{
class StatementHandle
{
	public:
		explicit StatementHandle(PreparedStatement &stmt) : stmt_(stmt)
		{
		}
		~StatementHandle() { stmt_.Reset(); }

		PreparedStatement *operator->() const { return &stmt_; }
		PreparedStatement &operator*() const { return stmt_; }

	private:
		PreparedStatement &stmt_;
};

class Connection
{
	public:
		Connection() = default;
		~Connection();

		Connection(const Connection &) = delete; // no copies
		Connection &operator=(const Connection &) = delete;
		Connection(Connection &&) noexcept = default; // allow moves
		Connection &operator=(Connection &&) noexcept = default;

		SQLiteError Open(const std::filesystem::path &path, int flags);
		tl::expected<StatementHandle, SQLiteError> Prepare(const std::string &sql);

		[[nodiscard]] sqlite3 *GetSq() const
		{
			return db_;
		}

		std::string_view errstr()
		{
			return sqlite3_errmsg(db_);
		}

		SQLiteError exec(const std::string &sql) const
		{
			return static_cast<SQLiteError>(sqlite3_exec(db_,
			                                             sql.c_str(),
			                                             nullptr,
			                                             nullptr,
			                                             nullptr));
		}

		SQLiteError execAll(const std::initializer_list<std::string> sqls)
		{
			for (const std::string &sql : sqls)
			{
				const SQLiteError ret = exec(sql);

				if (ret != Lite_Ok)
				{
					Logger::Log(err, "SQLite failed to exec: {}", errstr());
					return ret;
				}
			}
			return Lite_Ok;
		}

	private:
		sqlite3 *db_{nullptr};
		std::unordered_map<std::string, PreparedStatement> prepared_statements_;
};
} // namespace SQLite

#endif //SQLITECONNECTION_H
