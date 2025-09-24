//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <RadioChannel.h>

#include "connection/ConnectionPool.h"
#include "connection/PreparedStatement.h"
#include "utils/Expected.hpp"

class Table
{
	public:
		explicit Table(std::shared_ptr<SQLite::Connection> conn)
			: conn_(std::move(conn))
		{
		}
		virtual ~Table() = default;

	protected:
		template<typename ResultT, typename... T>
		tl::expected<ResultT, SQLiteError> QueryData(
			const std::string &sql,
			std::function<void(const SQLite::StatementHandle &, ResultT &)> converter,
			T &&... pairs) const
		{
			auto stmt_ref = conn_->Prepare(sql);
			if (!stmt_ref)
			{
				Logger::Log(debug, "SQLite: Unable to prepare: {}", conn_->errstr());
				return tl::unexpected(stmt_ref.error());
			}

			auto &stmt = *stmt_ref;

			SQLiteError ret = stmt->BindAll(std::forward<T>(pairs)...);
			if (ret != Lite_Ok)
			{
				Logger::Log(err, "SQLite: Unable to bind: {}", conn_->errstr());
				return tl::unexpected(ret);
			}

			stmt->LogSqlStatement();

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
			auto stmt_ref = conn_->Prepare(sql);
			if (!stmt_ref)
			{
				Logger::Log(debug, "SQLite: Unable to prepare: {}", conn_->errstr());
				return tl::unexpected(stmt_ref.error());
			}

			const auto &stmt = *stmt_ref;

			SQLiteError ret = stmt->BindAll(std::forward<T>(pairs)...);
			if (ret != Lite_Ok)
			{
				Logger::Log(err, "SQLite: Unable to bind: {}", conn_->errstr());
				return tl::unexpected(ret);
			}

			stmt->LogSqlStatement();

			ret = stmt->Step();
			if (ret == SQLITE_DONE)
				return {};

			return tl::unexpected(ret);
		}

		std::shared_ptr<SQLite::Connection> conn_;
};

#endif //DATABASE_H
