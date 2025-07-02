//
// Created by TheDaChicken on 6/9/2025.
//

#ifndef PREPAREDSTATEMENTS_H
#define PREPAREDSTATEMENTS_H

#include "sql/Connection.h"

class PreparedStatement
{
	public:
		explicit PreparedStatement(SQLite::Connection &connection) : connection(connection)
		{
		}

		bool Prepare(const std::string &sql)
		{
			connection.Prepare(sql, stmt_);
			return stmt_ != nullptr;
		}

		int BindText(const std::string_view key,
		             const std::string_view value) const
		{
			sqlite3_bind_text(stmt_,
			                  sqlite3_bind_parameter_index(stmt_, key.data()),
			                  value.data(),
			                  static_cast<int>(value.size()),
			                  SQLITE_STATIC);
			return sqlite3_errcode(connection.Get());
		}

		int BindInt(const std::string_view key, const int value) const
		{
			sqlite3_bind_int(stmt_,
			                 sqlite3_bind_parameter_index(stmt_, key.data()),
			                 value);
			return sqlite3_errcode(connection.Get());
		}

		int BindInt64(const std::string_view key, const int64_t value) const
		{
			sqlite3_bind_int64(stmt_,
			                   sqlite3_bind_parameter_index(stmt_, key.data()),
			                   value);
			return sqlite3_errcode(connection.Get());
		}

		std::string_view ReadString(const int index) const
		{
			return {
				reinterpret_cast<const char *>(sqlite3_column_text(stmt_, index)),
				static_cast<std::size_t>(sqlite3_column_bytes(stmt_, index))
			};
		}

		int ReadInt(const int index) const
		{
			return sqlite3_column_int(stmt_, index);
		}

		int64_t ReadInt64(const int index) const
		{
			return sqlite3_column_int64(stmt_, index);
		}

		int Step()
		{
			return sqlite3_step(stmt_);
		}

		[[nodiscard]] sqlite3_stmt *Get() const
		{
			return stmt_;
		}

		[[nodiscard]] sqlite3 *GetConnection() const
		{
			return connection.Get();
		}

		~PreparedStatement()
		{
			if (stmt_)
			{
				sqlite3_clear_bindings(stmt_);
				sqlite3_reset(stmt_);
			}
		}

	private:
		SQLite::Connection &connection;
		sqlite3_stmt *stmt_{nullptr};
};

#endif //PREPAREDSTATEMENTS_H
