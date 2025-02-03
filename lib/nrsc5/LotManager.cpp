//
// Created by TheDaChicken on 8/20/2024.
//

#include "LotManager.h"

#include "utils/StringUtils.h"
#include "utils/Log.h"

#include <fstream>
#include <QSqlQuery>
#include <QSqlError>

/* According to the White papers:
 * Station logos can be stored in non-volatile or volatile memory available,
 * cross-referenced by call sign & program number so that acquisition time is nominally very fast.
 */
constexpr auto kCreateLOTTable = QLatin1String(R"(
	CREATE TABLE IF NOT EXISTS HDRADIO_LOTS (
		callSign INTEGER NOT NULL,
     	lotId INTEGER NOT NULL,
		mime INTEGER NOT NULL,
		path TEXT NOT NULL,
     	expire INTEGER NOT NULL,
     	service INTEGER NOT NULL,
     	PRIMARY KEY (callSign, lotId)
	);
	)");

constexpr auto kCreateHDProgram = QLatin1String(R"(
	CREATE TABLE IF NOT EXISTS HDRADIO_PROGRAMS (
		callSign TEXT,
		program_id_ INTEGER,
		stationLogoLot INTEGER,
		PRIMARY KEY (callSign, program_id_),
		FOREIGN KEY (stationLogoLot) REFERENCES HDRADIO_LOTS(lotId)
	);
	)");

LotManager::LotManager(ConnectionManager *connection_manager)
	: connection_manager_(connection_manager)
{
	assert(this->connection_manager_);
}

int LotManager::Open()
{
	return this->connection_manager_->Transaction([&](const QSqlDatabase &db) -> bool
	{
		QSqlQuery query(db);
		if (!query.exec(kCreateHDProgram) || !query.exec(kCreateLOTTable))
		{
			Logger::Log(err, "Unable to create Lot tables {}", query.lastError().text());
			return false;
		}
		return true;
	});
}

int LotManager::SetImageFolder(const std::filesystem::path &imagePath)
{
	const auto permissions = status(imagePath).permissions();
	if ((permissions & std::filesystem::perms::owner_write) == std::filesystem::perms::none)
	{
		return -1;
	}

	try
	{
		create_directory(imagePath);
	} catch (const std::exception &e)
	{
		Logger::Log(err, "LotManager: Failed to create directory for images: {}", e.what());
		return -1;
	}

	image_path_ = imagePath;
	return 0;
}

std::filesystem::path LotManager::StationFolder(const NRSC5::Station &station) const
{
	const std::string folderName = station.country_code + "_" + std::to_string(station.id);
	return image_path_ / folderName;
}

bool LotManager::GetLot(const NRSC5::Station &station, NRSC5::Lot &lot) const
{
	if (!SQLGetLot(station, lot))
	{
		return false;
	}

	if (!exists(lot.path))
	{
		Logger::Log(warn,
		            "LotManager: Tried to access {}. Deleted file {}",
		            lot.id,
		            lot.path.string());
		// Clean up the database of that file if it's missing from the disk
		SQLDeleteLotsFromFile(station, lot.path, false);
		return false;
	}

	if (!is_regular_file(lot.path))
	{
		Logger::Log(warn,
		            "LotManager: Tried to access {}. Not a file {}",
		            lot.id,
		            lot.path.string());
		// Clean up the database of that file if it's not a file
		SQLDeleteLotsFromFile(station, lot.path, false);
		PhysicallyDelete(lot.path);
		return false;
	}

	if (lot.isExpired())
	{
		Logger::Log(warn,
		            "LotManager: Tried to access {}. Expired lot {}",
		            lot.id,
		            lot.path.string());
		SQLDeleteLotsFromFile(station, lot.path, true);
		return false;
	}
	return true;
}

/**
 * Finds lots with the same path and deletes them if they are expired
 *
 * If expire_only is false, all lots with the same path will be deleted including the physical the file.
 * If true, only expired lots will be deleted.
 * @param station The station that the lot belongs to
 * @param path The path of the lot to delete
 * @param expire_only If true, only expired lots will be deleted
 */
void LotManager::SQLDeleteLotsFromFile(const NRSC5::Station &station, const std::filesystem::path &path,
                                       const bool expire_only) const
{
	// Check if we have any more lots with the same path
	std::vector<NRSC5::Lot> lots;
	SQLGetFile(station, path, lots);

	connection_manager_->Transaction([this, lots, station, expire_only](QSqlDatabase &)
	{
		for (const auto &lot : lots)
		{
			if (expire_only && !lot.isExpired())
			{
				continue;
			}

			SQLDeleteLot(station, lot.id);
		}
		return true;
	});
}

void LotManager::LotReceived(const NRSC5::Station &station,
                             const NRSC5::DataService &component,
                             const NRSC5::Lot &lot) const
{
	// Check if we already have this lot
	if (IsLotAlreadyStored(station, lot))
	{
		Logger::Log(debug, "LotManager: We already have this lot {}", lot.id);

		// Make sure if we save special mime, we save it for quick access
		if (!SaveSpecialMIME(station, component, lot))
		{
			Logger::Log(err, "LotManager: Failed to save special MIME for lot {}", lot.name);
		}
		return;
	}

	const std::filesystem::path stationFolder = StationFolder(station);
	const std::filesystem::path path = stationFolder / lot.name;

	// Create the station folder if it doesn't exist
	if (!exists(stationFolder))
	{
		try
		{
			create_directory(stationFolder);
		} catch (const std::exception &e)
		{
			Logger::Log(err, "LotManager: Failed to create directory for lot {}: {}", lot.name, e.what());
			return;
		}
	}

	if (int ret = SaveDisk(path, lot.data); ret < 0)
	{
		Logger::Log(err, "LotManager: Failed to save lot {} to disk: {}", lot.name, ret);
		return;
	}

	if (SQLInsertLot(station, component, lot, path))
	{
		Logger::Log(info, "LotManager: Saved lot {}", lot.name);

		if (!SaveSpecialMIME(station, component, lot))
		{
			Logger::Log(err, "LotManager: Failed to save special MIME for lot {}", lot.name);
		}
	}
	else
	{
		// Delete the lot file if we failed to save it to the database
		PhysicallyDelete(path);
	}
}

bool LotManager::SaveSpecialMIME(const NRSC5::Station &station,
                                 const NRSC5::DataService &component,
                                 const NRSC5::Lot &lot) const
{
	if (component.mime == NRSC5_MIME_STATION_LOGO)
	{
		// Station Logo is a special MIME.
		// We need to store this for quick access to the logo
		return SQLInsertStationLogo(station, lot, component.program.value());
	}
	return true;
}

bool LotManager::IsLotAlreadyStored(const NRSC5::Station &station, const NRSC5::Lot &lot) const
{
	NRSC5::Lot oldLot;
	oldLot.id = lot.id;

	return SQLGetLot(station, oldLot)
			&& oldLot.expire_point == lot.expire_point
			&& oldLot.mime == lot.mime;
}

bool LotManager::SQLInsertLot(const NRSC5::Station &station,
                              const NRSC5::DataService &component,
                              const NRSC5::Lot &lot, const std::filesystem::path &path) const
{
	assert(this->connection_manager_);

	static constexpr auto kInsertHDRadioLot = QLatin1String(R"(
		INSERT OR REPLACE INTO HDRADIO_LOTS VALUES
			(:callSign, :lotId, :mime, :path, :expire, :service)
	)");

	QSqlQuery *query(connection_manager_->GetPreparedQuery(kInsertHDRadioLot));

	if (!query)
		return false;

	query->bindValue(":callSign", QString::fromStdString(station.name));
	query->bindValue(":lotId", lot.id);
	query->bindValue(":mime", lot.mime);
	query->bindValue(":path", StringUtils::PathToStr(path));
	query->bindValue(":expire",
	                 static_cast<qulonglong>(std::chrono::duration_cast<std::chrono::seconds>(
		                 lot.expire_point.time_since_epoch()).count()));
	query->bindValue(":service", component.mime);

	if (!query->exec())
	{
		Logger::Log(err,
		            "LotManager: Failed to insert lot {} to database: {}",
		            lot.name,
		            query->lastError().text());
		query->finish();
		return false;
	}

	query->finish();
	return true;
}

bool LotManager::SQLInsertStationLogo(const NRSC5::Station &station,
                                      const NRSC5::Lot &lot,
                                      const unsigned int programId) const
{
	assert(this->connection_manager_);

	static constexpr auto kInsertHDRadioProgram = QLatin1String(R"(
		INSERT OR REPLACE INTO HDRADIO_PROGRAMS VALUES (:callSign, :programId, :stationLogoLot)
	)");

	QSqlQuery *query(connection_manager_->GetPreparedQuery(kInsertHDRadioProgram));

	if (!query)
		return false;

	// Save logo service lot to database
	query->bindValue(":callSign", station.name.data());
	query->bindValue(":programId", programId);
	query->bindValue(":stationLogoLot", lot.id);

	if (!query->exec())
	{
		Logger::Log(err,
		            "LotManager: Failed to insert logo {} to database: {}",
		            lot.name,
		            query->lastError().text());
		query->finish();
		return false;
	}

	query->finish();
	return true;
}

bool LotManager::SQLGetStationImage(const NRSC5::Station &station, NRSC5::Lot &lot) const
{
	assert(this->connection_manager_);

	static constexpr auto kFindHDRadioProgram = QLatin1String(R"(
		SELECT * FROM HDRADIO_PROGRAMS WHERE callSign = ? AND program_id_ = ?
	)");

	QSqlQuery *logo_find_query_(connection_manager_->GetPreparedQuery(kFindHDRadioProgram));

	logo_find_query_->bindValue(0, QString::fromStdString(station.name));
	logo_find_query_->bindValue(1, station.current_program);

	if (!logo_find_query_->exec())
	{
		Logger::Log(err,
		            "LotManager: Failed to find {} station image: {}",
		            station.name,
		            logo_find_query_->lastError().text());
		return false;
	}

	if (!logo_find_query_->next())
	{
		logo_find_query_->finish();
		return false;
	}

	lot.id = logo_find_query_->value(2).toInt();

	// Clear query
	logo_find_query_->finish();

	// Get lot
	return GetLot(station, lot);
}

bool LotManager::SQLGetLot(const NRSC5::Station &station, NRSC5::Lot &lot) const
{
	assert(lot.id != 0);
	assert(this->connection_manager_);

	static constexpr auto kFindHDRadioLot = QLatin1String(R"(
		SELECT * FROM HDRADIO_LOTS WHERE callSign = :callSign AND lotId = :lotId
	)");

	QSqlQuery *query(connection_manager_->GetPreparedQuery(kFindHDRadioLot));

	if (!query)
		return false;

	query->bindValue(":callSign", QString::fromStdString(station.name));
	query->bindValue(":lotId", lot.id);

	if (!query->exec())
	{
		Logger::Log(err, "LotManager: Failed to exec statement: {}", query->lastError().text());
		return false;
	}

	// Check if the lot exists
	if (!query->next())
	{
		query->finish();
		return false;
	}

	SQLtoLot(*query, lot);

	query->finish();
	return true;
}

void LotManager::SQLGetFile(const NRSC5::Station &station, const std::filesystem::path &path,
                            std::vector<NRSC5::Lot> &lots) const
{
	assert(this->connection_manager_);

	static constexpr auto kDeleteHDRadioLot = QLatin1String(R"(
		SELECT * FROM HDRADIO_LOTS WHERE callSign = :callSign AND path = :path
	)");

	QSqlQuery *query(connection_manager_->GetPreparedQuery(kDeleteHDRadioLot));

	if (!query)
		return;

	query->bindValue(":callSign", QString::fromStdString(station.name));
	query->bindValue(":path", StringUtils::PathToStr(path));

	if (!query->exec())
	{
		Logger::Log(err, "LotManager: Failed to exec statement: {}", query->lastError().text());
		query->finish();
		return;
	}

	while (query->next())
	{
		NRSC5::Lot lot;
		SQLtoLot(*query, lot);
		lots.push_back(lot);
	}

	query->finish();
}

void LotManager::SQLDeleteLot(const NRSC5::Station &station,
                              unsigned int lot) const
{
	assert(this->connection_manager_);

	static constexpr auto kDeleteHDRadioLot = QLatin1String(R"(
		DELETE FROM HDRADIO_LOTS WHERE callSign = :callSign AND lotId = :lotId
	)");

	QSqlQuery *delete_query(connection_manager_->GetPreparedQuery(kDeleteHDRadioLot));

	delete_query->bindValue(":callSign", QString::fromStdString(station.name));
	delete_query->bindValue(":lotId", lot);

	delete_query->exec();
	delete_query->finish();

	Logger::Log(warn, "LotManager: Deleted lot {}", lot);
}

void LotManager::SQLtoLot(const QSqlQuery &query, NRSC5::Lot &lot)
{
	lot.id = query.value(1).toInt();
	lot.mime = query.value(2).toInt();
	lot.path = StringUtils::StrToPath(query.value(3).toString());
	lot.expire_point = std::chrono::system_clock::time_point(
		std::chrono::seconds{query.value(4).toLongLong()}
	);
}

int LotManager::SaveDisk(const std::filesystem::path &path, const vector_uint8_t &vector)
{
	std::ofstream image(path, std::ios_base::out | std::ios_base::binary);
	if (image.fail())
	{
		Logger::Log(err,
		            "LotManager: Unable to create open stream for {}: {}",
		            path.string(),
		            strerror(errno));
		return -1;
	}

	image.write(reinterpret_cast<const char *>(vector.data()),
	            static_cast<std::streamsize>(vector.size()));
	return 0;
}

void LotManager::PhysicallyDelete(const std::filesystem::path &path)
{
	try
	{
		std::filesystem::remove(path);
	} catch (const std::exception &e)
	{
		Logger::Log(err, "LotManager: Failed to remove file {}: {}", path.string(), e.what());
	}
}
