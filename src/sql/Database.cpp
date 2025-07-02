//
// Created by TheDaChicken on 2/16/2025.
//

#include "Database.h"

#include <filesystem>
#include <sql/sqlite/sqlite3.h>

#include "SqliteHelper.h"
#include "utils/Error.h"

static constexpr int SCHEMA_VERSION = 1;

static constexpr auto kCreateSettingsTable = R"(
	CREATE TABLE IF NOT EXISTS SETTINGS (
		key TEXT PRIMARY KEY,
		value TEXT
	);
)";

static constexpr auto kCreateSchemaVersion = R"(
	CREATE TABLE IF NOT EXISTS SCHEMA_VERSION (
		version INTEGER PRIMARY KEY
	);
)";

/* According to the White papers:
 * Station logos can be stored in non-volatile or volatile memory available,
 * cross-referenced by call sign & program number so that acquisition time is nominally very fast.
 */

constexpr auto kCreateLOTTable = R"(
	CREATE TABLE IF NOT EXISTS HDRADIO_LOTS (
		callSign INTEGER NOT NULL,
		channel INTEGER NOT NULL,
     	service INTEGER NOT NULL,
     	lotId INTEGER NOT NULL,
		mime INTEGER NOT NULL,
		path TEXT NOT NULL,
     	expire INTEGER NOT NULL,
     	PRIMARY KEY (callSign, service, channel, lotId)
	);
)";

static constexpr auto kCreateStationLogoUniqueLOTIndex = R"(
	CREATE UNIQUE INDEX IF NOT EXISTS HDRADIO_STATION_LOGO
	ON HDRADIO_LOTS (callSign, service, channel)
	WHERE service={:d}
)";

static constexpr auto kInsertSetting = R"(
	INSERT OR REPLACE INTO SETTINGS (key, value) VALUES (:key, :value);
)";

static constexpr auto kGetSettingValue = R"(
	SELECT * FROM SETTINGS WHERE key = :key;
)";

static constexpr auto kGetSchemaVersion = R"(
	SELECT * FROM SCHEMA_VERSION;
)";

static constexpr auto kInsertSchemaVersion = R"(
	INSERT OR REPLACE INTO SCHEMA_VERSION (version) VALUES (:version);
)";

static constexpr auto kInsertHDRadioLot = R"(
	INSERT OR REPLACE INTO HDRADIO_LOTS (
		callSign, channel, service, lotId, mime, path, expire
	) VALUES (
		:callSign, :channel, :service, :lotId, :mime, :path, :expire
	);
)";

static constexpr auto kGetHDRadioLot = R"(
	SELECT * FROM HDRADIO_LOTS
	WHERE
		callSign = :callSign AND
		channel = :channel AND
		service = :service AND
		lotId = :lotId
	;
)";

static constexpr auto kDeleteHDRadioLot = R"(
	DELETE FROM HDRADIO_LOTS WHERE
		callSign = :callSign AND
		channel = :channel AND
		service = :service AND
		lotId = :lotId
	;
)";

static constexpr auto kGetHDRadioLotsPerService = R"(
	SELECT * FROM HDRADIO_LOTS
	WHERE
		callSign = :callSign AND
		channel = :channel AND
		service = :service
	;
)";

UTILS::StatusCodes SQLite::Database::Open(const std::string &path)
{
	if (pool_.Open(path) != UTILS::StatusCodes::Ok)
	{
		Logger::Log(debug, "SQLite: Unable to open database");
		return UTILS::StatusCodes::DatabaseError;
	}

	if (CreateSchemaVersion() == UTILS::StatusCodes::DatabaseError)
	{
		return UTILS::StatusCodes::DatabaseError;
	}

	int version;
	if (GetSchemaVersion(version) == UTILS::StatusCodes::Empty)
	{
		return CreateSchema();
	}

	if (SCHEMA_VERSION != version)
	{
		Logger::Log(debug, "SQLite: Schema version mismatch: {} != {}", SCHEMA_VERSION, version);
		return UTILS::StatusCodes::DatabaseError;
	}

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes SQLite::Database::CreateSchema()
{
	int ret;
	sqlite3 *db = pool_.GetConnection().Get();

	SQLITE_RETURN_ERRMSG(sqlite3_exec(db, kCreateSettingsTable,
		                     nullptr, nullptr, nullptr),
	                     "Unable to create Favorite Channels table");
	SQLITE_RETURN_ERRMSG(sqlite3_exec(db, kCreateLOTTable,
		                     nullptr, nullptr, nullptr),
	                     "Unable to create HDRadio LOT table");
	SQLITE_RETURN_ERRMSG(sqlite3_exec(db,
		                     fmt::format(kCreateStationLogoUniqueLOTIndex, NRSC5_MIME_STATION_LOGO).data(),
		                     nullptr, nullptr, nullptr),
	                     "Unable to create Unique Lot Index table");

	return SetSchemaVersion(SCHEMA_VERSION);
}

UTILS::StatusCodes SQLite::Database::CreateSchemaVersion()
{
	int ret;
	sqlite3 *db = pool_.GetConnection().Get();

	SQLITE_RETURN_ERRMSG(sqlite3_exec(db, kCreateSchemaVersion,
		                     nullptr, nullptr, nullptr),
	                     "Unable to create Schema Version table");

	return UTILS::StatusCodes::Ok;
}

UTILS::StatusCodes SQLite::Database::GetSchemaVersion(int &version)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();

	if (statement.Prepare(kGetSchemaVersion) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare GET Schema Version {}", sqlite3_errmsg(db));
		return {};
	}

	int ret = sqlite3_step(statement.Get());
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Empty;
	if (ret == SQLITE_ROW)
	{
		version = sqlite3_column_int(statement.Get(), 0);
		return UTILS::StatusCodes::Ok;
	}
	else
	{
		Logger::Log(debug, "SQLite: Unable to Get Schema Version {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::SetSchemaVersion(const int version)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	if (statement.Prepare(kInsertSchemaVersion) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Insert SchemaVersion {}", sqlite3_errmsg(db));
		return {};
	}

	SQLITE_RETURN_ERRMSG(sqlite3_bind_int(statement.Get(),
		                     sqlite3_bind_parameter_index(statement.Get(), ":version"), version),
	                     "Unable to set Insert SchemaVersion");

	ret = sqlite3_step(statement.Get());
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Ok;
	else
	{
		Logger::Log(debug, "SQLite: Unable to Insert SchemaVersion {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::GetSettingValue(
	const std::string_view key,
	std::string &value)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	if (statement.Prepare(kGetSettingValue) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Get Setting {}", sqlite3_errmsg(db));
		return {};
	}

	SQLITE_RETURN_ERRMSG(statement.BindText(":key", key), "Unable to set Setting Key");

	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Empty;
	if (ret == SQLITE_ROW)
	{
		value = statement.ReadString(1);
		return UTILS::StatusCodes::Ok;
	}
	else
	{
		Logger::Log(debug, "SQLite: Unable to Get Setting {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::SetSettingValue(
	const std::string_view key,
	const std::string_view value)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	if (statement.Prepare(kInsertSetting) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Set Setting Value {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}

	SQLITE_RETURN_ERRMSG(statement.BindText(":key", key), "Unable to set Setting key");
	SQLITE_RETURN_ERRMSG(statement.BindText(":value", value), "Unable to set Setting value");

	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Ok;
	else
	{
		Logger::Log(debug, "SQLite: Unable to Insert Setting {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::InsertLot(
	const NRSC5::StationInfo &station,
	const NRSC5::Lot &lot,
	const std::filesystem::path &path)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	const std::string path_str = path.string();
	int ret;

	if (statement.Prepare(kInsertHDRadioLot) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Insert LOT {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}

	const auto expire = std::chrono::duration_cast<std::chrono::seconds>(
		lot.expire_point.time_since_epoch()).count();

	assert(lot.id > 0);
	assert(lot.component.channel > 0);

	SQLITE_RETURN_ERRMSG(statement.BindText(":callSign", station.name),
	                     "Unable to set LOT callSign for INSERT");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":channel", lot.component.channel),
	                     "Unable to set LOT channel for INSERT");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":service", lot.component.mime),
	                     "Unable to set LOT service for INSERT");

	SQLITE_RETURN_ERRMSG(statement.BindInt(":lotId", lot.id),
	                     "Unable to set Lot ID for INSERT");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":mime", lot.mime),
	                     "Unable to set Lot Mime for INSERT");;
	SQLITE_RETURN_ERRMSG(statement.BindText(":path", path_str),
	                     "Unable to set Lot Path for INSERT");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":expire", expire),
	                     "Unable to set Lot Expire for INSERT");
	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Ok;
	else
	{
		Logger::Log(debug, "SQLite: Unable to Insert Lot {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::GetLot(
	const NRSC5::StationInfo &station,
	NRSC5::Lot &lot)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	assert(lot.id > 0);
	assert(lot.component.channel > 0);

	if (statement.Prepare(kGetHDRadioLot) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Get Lot {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}

	SQLITE_RETURN_ERRMSG(statement.BindText(":callSign", station.name),
	                     "Unable to set Get LOT CallSign");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":channel", lot.component.channel),
	                     "Unable to set Get Lot Channel");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":service", lot.component.mime),
	                     "Unable to set Get Lot Service");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":lotId", lot.id),
	                     "Unable to set Get Lot ID");

	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Empty;
	if (ret == SQLITE_ROW)
	{
		assert(lot.id == sqlite3_column_int(statement.Get(), 3));
		ConvertToLot(statement, lot);
		return UTILS::StatusCodes::Ok;
	}
	else
	{
		Logger::Log(debug, "SQLite: Unable to Get Lot {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}
}

UTILS::StatusCodes SQLite::Database::DeleteLot(
	const NRSC5::StationInfo &station,
	const NRSC5::Lot &lot)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	if (statement.Prepare(kDeleteHDRadioLot) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Delete Lot {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}

	SQLITE_RETURN_ERRMSG(statement.BindText(":callSign", station.name),
	                     "Unable to set Delete LOT callSign");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":channel", lot.component.channel),
	                     "Unable to set Delete Lot Channel");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":service", lot.component.mime),
	                     "Unable to set Delete Lot Service");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":lotId", lot.id),
	                     "Unable to set Delete Lot ID");

	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Ok;
	else
		return UTILS::StatusCodes::DatabaseError;
}

UTILS::StatusCodes SQLite::Database::GetLotSpecial(
	const NRSC5::StationInfo &station,
	NRSC5::Lot &lot)
{
	PreparedStatement statement(pool_.GetConnection());
	sqlite3 *db = statement.GetConnection();
	int ret;

	assert(lot.component.channel > 0);

	if (statement.Prepare(kGetHDRadioLotsPerService) == false)
	{
		Logger::Log(debug, "SQLite: Unable to prepare Get HDRadio Special {}", sqlite3_errmsg(db));
		return UTILS::StatusCodes::DatabaseError;
	}

	SQLITE_RETURN_ERRMSG(statement.BindText(":callSign", station.name),
	                     "Unable to set Get LOTs CallSign");
	SQLITE_RETURN_ERRMSG(statement.BindInt(":channel", lot.component.channel),
	                     "Unable to set Get LOTs Channel");
	SQLITE_RETURN_ERRMSG(statement.BindInt64(":service", lot.component.mime),
	                     "Unable to set Get LOTs Service");

	ret = statement.Step();
	if (ret == SQLITE_OK || ret == SQLITE_DONE)
		return UTILS::StatusCodes::Empty;
	else if (ret == SQLITE_ROW)
	{
		ConvertToLot(statement, lot);
		return UTILS::StatusCodes::Ok;
	}
	else
		return UTILS::StatusCodes::DatabaseError;
}

void SQLite::Database::ConvertToLot(
	const PreparedStatement &stmt,
	NRSC5::Lot &lot)
{
	lot.id = stmt.ReadInt(3);
	lot.mime = stmt.ReadInt(4);
	lot.path = stmt.ReadString(5);
	lot.expire_point = std::chrono::system_clock::time_point(
		std::chrono::seconds{stmt.ReadInt64(6)});
}
