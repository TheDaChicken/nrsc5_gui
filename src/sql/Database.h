//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <RadioChannel.h>

#include "ConnectionPool.h"
#include "sqlite/sqlite3.h"
#include "Utils/Error.h"

namespace SQLite
{
class Database
{
	public:
		Database() = default;
		~Database() = default;

		Q_DISABLE_COPY_MOVE(Database)

		UTILS::StatusCodes Open(const std::string &path);
		UTILS::StatusCodes GetSchemaVersion(int &version);
		UTILS::StatusCodes SetSchemaVersion(int version);

		UTILS::StatusCodes GetSettingValue(std::string_view key, std::string &value);
		UTILS::StatusCodes SetSettingValue(std::string_view key, std::string_view value);

		UTILS::StatusCodes InsertLot(const NRSC5::StationInfo &station,
		                             const NRSC5::DataService &component,
		                             const NRSC5::Lot &lot, const std::filesystem::path &path);
		UTILS::StatusCodes GetLotSpecial(const NRSC5::StationInfo &station, const NRSC5::DataService &component,
		                                 NRSC5::Lot &lot);

		UTILS::StatusCodes GetLot(const NRSC5::StationInfo &station, const NRSC5::DataService &component,
		                          NRSC5::Lot &lot);
		UTILS::StatusCodes DeleteLot(const NRSC5::StationInfo &station, const NRSC5::Lot &lot);

	private:
		UTILS::StatusCodes CreateSchemaVersion();
		UTILS::StatusCodes CreateSchema();

		static void ConvertToLot(sqlite3_stmt *stmt, NRSC5::Lot &lot);
		static std::string ReadString(sqlite3_stmt *stmt, int index);

		ConnectionPool pool_;
};
}

#endif //SQLITEDATABASE_H
