//
// Created by TheDaChicken on 6/9/2025.
//

#ifndef PREPAREDSTATEMENTS_H
#define PREPAREDSTATEMENTS_H

#include "SqliteHelper.h"
#include "../sqlite/sqlite3.h"
#include "utils/Log.h"

#include <cassert>

class PreparedStatement
{
	public:
		explicit PreparedStatement(sqlite3_stmt *stmt)
			: stmt_(stmt)
		{
			assert(stmt_);
		}

		~PreparedStatement()
		{
			assert(stmt_);
			sqlite3_finalize(stmt_);
		}

		void Reset() const
		{
			sqlite3_clear_bindings(stmt_);
			sqlite3_reset(stmt_);
		}

		PreparedStatement(const PreparedStatement &) = delete; // no copy
		PreparedStatement &operator=(const PreparedStatement &) = delete; // no copy-
		PreparedStatement(PreparedStatement &&) = default; // allow move construction
		PreparedStatement &operator=(PreparedStatement &&other)
		{
			if (this != &other)
			{
				if (stmt_)
					sqlite3_finalize(stmt_);
				stmt_ = other.stmt_;
				other.stmt_ = nullptr;
			}
			return *this;
		}

		template<typename T>
		SQLiteError Bind(const std::string &key, const T &value) const
		{
			const int parameter = GetParameterIndex(key.c_str());
			if (parameter == 0)
				return Lite_Misuse;

			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
				return static_cast<SQLiteError>(sqlite3_bind_text(stmt_,
				                                                  parameter,
				                                                  value.data(),
				                                                  static_cast<int>(value.size()),
				                                                  SQLITE_STATIC));
			else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t>)
			{
				return static_cast<SQLiteError>(sqlite3_bind_int64(stmt_,
				                                                   parameter,
				                                                   value));
			}
			else if constexpr (std::is_same_v<T, int>)
			{
				return static_cast<SQLiteError>(sqlite3_bind_int(stmt_,
				                                                 parameter,
				                                                 value));
			}
			else
			{
				static_assert(sizeof(T) == 0, "Unsupported type for BindText");
				return Lite_Misuse;
			}
		}

		template<typename... T>
		SQLiteError BindAll(T &&... pair)
		{
			SQLiteError ret = Lite_Ok;

			([&]
			{
				if (ret != Lite_Ok)
					return;

				ret = Bind(pair.first, pair.second);
				sqlite3 *db_ = sqlite3_db_handle(stmt_);

				if (ret != Lite_Ok)
				{
					Logger::Log(err,
					            "SQLite: Unable to bind {}: {}",
					            pair.first,
					            sqlite3_errmsg(db_));
				}
			}(), ...);

			return ret;
		}

		int GetParameterIndex(std::string_view key) const
		{
			return sqlite3_bind_parameter_index(stmt_, key.data());
		}

		[[nodiscard]] std::string_view GetColumnString(const int index) const
		{
			return {
				reinterpret_cast<const char *>(sqlite3_column_text(stmt_, index)),
				static_cast<std::size_t>(sqlite3_column_bytes(stmt_, index))
			};
		}

		SQLiteError GetColumnInt(const int index) const
		{
			return static_cast<SQLiteError>(sqlite3_column_int(stmt_, index));
		}

		int64_t GetColumnInt64(const int index) const
		{
			return sqlite3_column_int64(stmt_, index);
		}

		SQLiteError Step()
		{
			return static_cast<SQLiteError>(sqlite3_step(stmt_));
		}

		void LogSqlStatement()
		{
			// Write the expected sql result
			char *test = sqlite3_expanded_sql(stmt_);
			Logger::Log(debug, "SQLite Submitted: {}", test);
			sqlite3_free(test);
		}

		[[nodiscard]] sqlite3_stmt *Get() const
		{
			return stmt_;
		}

	private:
		sqlite3_stmt *stmt_{nullptr};
};

#endif //PREPAREDSTATEMENTS_H
