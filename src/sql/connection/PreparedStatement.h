//
// Created by TheDaChicken on 6/9/2025.
//

#ifndef PREPAREDSTATEMENTS_H
#define PREPAREDSTATEMENTS_H

#include "SqliteHelper.h"
#include "../sqlite/sqlite3.h"
#include "utils/Log.h"

#include <cassert>

using STMT_PTR = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

class PreparedStatement
{
	public:
		explicit PreparedStatement()
			: stmt_(nullptr, sqlite3_finalize)
		{
		}

		~PreparedStatement()
		{
		}

		int Create(sqlite3 *db, const std::string_view sql)
		{
			sqlite3_stmt *stmt;
			const int ret = sqlite3_prepare_v2(
				db,
				sql.data(),
				static_cast<int>(sql.size()),
				&stmt,
				nullptr);
			if (ret == SQLITE_OK)
				stmt_ = STMT_PTR(stmt, sqlite3_finalize);
			return ret;
		}

		void Reset() const
		{
			sqlite3_clear_bindings(stmt_.get());
			sqlite3_reset(stmt_.get());
		}

		PreparedStatement(const PreparedStatement &) = delete; // no copy
		PreparedStatement &operator=(const PreparedStatement &) = delete; // no copy-
		PreparedStatement(PreparedStatement &&) = default; // allow move construction
		PreparedStatement &operator=(PreparedStatement &&other)
		{
			if (this != &other)
			{
				stmt_ = std::move(other.stmt_);
				other.stmt_ = nullptr;
			}
			return *this;
		}

		template<typename T>
		SQLiteError Bind(const std::string_view key, const T &value) const
		{
			const int parameter = GetParameterIndex(key);
			if (parameter == 0)
				return Lite_Misuse;

			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
				return static_cast<SQLiteError>(sqlite3_bind_text(
					stmt_.get(),
					parameter,
					value.data(),
					static_cast<int>(value.size()),
					SQLITE_STATIC));
			else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t>)
			{
				return static_cast<SQLiteError>(
					sqlite3_bind_int64(
						stmt_.get(),
						parameter,
						value));
			}
			else if constexpr (std::is_same_v<T, int>)
			{
				return static_cast<SQLiteError>(sqlite3_bind_int(
					stmt_.get(),
					parameter,
					value));
			}
			else
			{
				static_assert(sizeof(T) == 0, "Unsupported type for Bind()");
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
				sqlite3 *db_ = sqlite3_db_handle(stmt_.get());

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

		[[nodiscard]] int GetParameterIndex(const std::string_view key) const
		{
			return sqlite3_bind_parameter_index(stmt_.get(), key.data());
		}

		[[nodiscard]] std::string_view GetColumnString(const int index) const
		{
			return {
				reinterpret_cast<const char *>(sqlite3_column_text(stmt_.get(), index)),
				static_cast<std::size_t>(sqlite3_column_bytes(stmt_.get(), index))
			};
		}

		[[nodiscard]] int GetColumnInt(const int index) const
		{
			return sqlite3_column_int(stmt_.get(), index);
		}

		[[nodiscard]] int64_t GetColumnInt64(const int index) const
		{
			return sqlite3_column_int64(stmt_.get(), index);
		}

		SQLiteError Step()
		{
			return static_cast<SQLiteError>(sqlite3_step(stmt_.get()));
		}

		std::string ExpectedString()
		{
			return {sqlite3_expanded_sql(stmt_.get())};
		}

		[[nodiscard]] sqlite3_stmt *Get() const
		{
			return stmt_.get();
		}

	private:
		STMT_PTR stmt_;
};

#endif //PREPAREDSTATEMENTS_H
