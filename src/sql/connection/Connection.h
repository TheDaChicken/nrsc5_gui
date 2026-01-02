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
		explicit StatementHandle(std::shared_ptr<PreparedStatement> stmt) : stmt_(std::move(stmt))
		{
		}
		~StatementHandle() { if (stmt_) stmt_->Reset(); }

		PreparedStatement *operator->() const { return stmt_.get(); }
		PreparedStatement &operator*() const { return *stmt_; }

	private:
		std::shared_ptr<PreparedStatement> stmt_;
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

		SQLiteError Execute(const std::string &data) const
		{
			return static_cast<SQLiteError>(sqlite3_exec(
				db_,
				data.c_str(),
				nullptr,
				nullptr,
				nullptr));
		}

		SQLiteError ExecuteAll(const std::initializer_list<std::string> &sqls) const
		{
			for (const std::string &sql : sqls)
			{
				SQLiteError ret = Execute(sql);
				if (ret != SQLITE_OK)
					return ret;
			}
			return Lite_Ok;
		}

		template<typename ResultT, typename... T>
		tl::expected<ResultT, SQLiteError> QueryData(
			const std::string &sql,
			std::function<void(const StatementHandle &, ResultT &)> converter,
			T &&... pairs)
		{
			auto stmt_ref = Prepare(sql);
			if (!stmt_ref)
			{
				Logger::Log(debug, "SQLite: Unable to prepare: {}", errstr());
				return tl::unexpected(stmt_ref.error());
			}

			auto &stmt = *stmt_ref;

			SQLiteError ret = stmt->BindAll(std::forward<T>(pairs)...);
			if (ret != Lite_Ok)
			{
				Logger::Log(err, "SQLite: Unable to bind: {}", errstr());
				return tl::unexpected(ret);
			}

			Logger::Log(debug, "SQLite Submitted: {}", stmt->ExpectedString());

			ret = stmt->Step();
			if (ret == SQLITE_ROW)
			{
				ResultT result{};
				converter(stmt, result);
				return result;
			}
			if (ret == SQLITE_DONE)
				return tl::unexpected(Lite_Done);

			return tl::unexpected(ret);
		}

		template<typename... T>
		tl::expected<void, SQLiteError> InsertData(
			const std::string &sql,
			T &&... pairs)
		{
			auto stmt_ref = Prepare(sql);
			if (!stmt_ref)
			{
				Logger::Log(debug, "SQLite: Unable to prepare: {}", errstr());
				return tl::unexpected(stmt_ref.error());
			}

			const auto &stmt = *stmt_ref;

			SQLiteError ret = stmt->BindAll(std::forward<T>(pairs)...);
			if (ret != Lite_Ok)
			{
				Logger::Log(err, "SQLite: Unable to bind: {}", errstr());
				return tl::unexpected(ret);
			}

			Logger::Log(debug, "SQLite Submitted: {}", stmt->ExpectedString());

			ret = stmt->Step();
			if (ret == SQLITE_DONE)
				return {};

			return tl::unexpected(ret);
		}

		[[nodiscard]] sqlite3 *GetSq() const
		{
			return db_;
		}

		std::string_view errstr() const
		{
			return sqlite3_errmsg(db_);
		}

	private:
		sqlite3 *db_{nullptr};
		std::unordered_map<std::string, std::shared_ptr<PreparedStatement> > prepared_statements_;
};
} // namespace SQLite

#endif //SQLITECONNECTION_H
