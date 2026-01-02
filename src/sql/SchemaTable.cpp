//
// Created by TheDaChicken on 8/29/2025.
//

#include "SchemaTable.h"

extern "C"
{
#include "nrsc5.h"
	}

constexpr int SCHEMA_VERSION = 1;

constexpr auto kCreateSchemaTable = R"(
	CREATE TABLE IF NOT EXISTS SCHEMA_INFO (
		version INTEGER NOT NULL
	);
)";

constexpr auto kCreateSettingsTable = R"(
	CREATE TABLE IF NOT EXISTS SETTINGS (
		key TEXT PRIMARY KEY,
		value TEXT
	);
)";

/* According to the White papers:
 * Station logos can be stored in non-volatile or volatile memory available,
 * cross-referenced by call sign & program number so that acquisition time is nominally very fast.
 */
constexpr auto kCreateLOTTable = R"(
	CREATE TABLE IF NOT EXISTS HDRADIO_LOTS (
		callSign TEXT NOT NULL,
		channel INTEGER NOT NULL,
     	service INTEGER NOT NULL,
     	lotId INTEGER NOT NULL,
		mime INTEGER NOT NULL,
		path TEXT NOT NULL,
     	expire INTEGER NOT NULL,
     	PRIMARY KEY (callSign, service, channel, lotId)
	);
)";

constexpr auto kCreateStationLogoUniqueLOTIndex = R"(
	CREATE UNIQUE INDEX IF NOT EXISTS HDRADIO_STATION_LOGO
	ON HDRADIO_LOTS (callSign, service, channel)
	WHERE service={:d}
)";

constexpr auto kInsertSchema = R"(
	INSERT INTO SCHEMA_INFO (version) VALUES (:version);
)";

constexpr auto kGetSchema = R"(
	SELECT version FROM SCHEMA_INFO LIMIT 1;
)";

tl::expected<void, SQLiteError> SchemaTable::CreateSchemaTable()
{
	const SQLiteError ret = conn_->ExecuteAll({kCreateSchemaTable});
	if (ret != Lite_Ok)
	{
		Logger::Log(err, "SQLite: Unable to create database schema");
		return tl::unexpected(ret);
	}
	return SetSchemaVersion(SCHEMA_VERSION);
}

tl::expected<void, SQLiteError> SchemaTable::CreateSchema()
{
	const SQLiteError ret = conn_->ExecuteAll({
		kCreateSettingsTable,
		kCreateLOTTable,
		fmt::format(kCreateStationLogoUniqueLOTIndex, NRSC5_MIME_STATION_LOGO)
	});
	if (ret != Lite_Ok)
	{
		Logger::Log(err, "SQLite: Unable to create database schema");
		return tl::unexpected(ret);
	}
	return SetSchemaVersion(SCHEMA_VERSION);
}

tl::expected<int, SQLiteError> SchemaTable::GetSchemaVersion()
{
	return conn_->QueryData<int>(kGetSchema,
	                      [](const SQLite::StatementHandle &stmt, int &out)
	                      {
		                      out = stmt->GetColumnInt(0);
	                      });
}

tl::expected<void, SQLiteError> SchemaTable::SetSchemaVersion(int version)
{
	return conn_->InsertData(kInsertSchema, std::make_pair(":version", version));
}

int SchemaTable::GetSupportedSchema() const
{
	return SCHEMA_VERSION;
}

