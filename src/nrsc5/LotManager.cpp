//
// Created by TheDaChicken on 8/20/2024.
//

#include "LotManager.h"

#include "utils/StringUtils.h"
#include "utils/Log.h"

#include <fstream>

LotManager::LotManager(SQLite::Database &db)
	: db_(db)
{
}

std::error_code LotManager::SetImageFolder(
	const std::filesystem::path &imagePath)
{
	try
	{
		create_directory(imagePath);
	} catch (const std::filesystem::filesystem_error &e)
	{
		Logger::Log(err, "LotManager: Failed to create directory for images: {}", e.what());
		return e.code();
	}

	image_path_ = imagePath;
	return {};
}

std::filesystem::path LotManager::StationFolder(
	const NRSC5::StationInfo &station) const
{
	const std::string folderName = station.country_code + "_" + std::to_string(station.id);
	return image_path_ / folderName;
}

bool LotManager::GetLot(
	const NRSC5::StationInfo &station,
	NRSC5::Lot &lot) const
{
	const UTILS::StatusCodes ret = db_.GetLot(station, lot);
	if (ret == UTILS::StatusCodes::Empty)
		return false;
	if (ret != UTILS::StatusCodes::Ok)
	{
		Logger::Log(err, "LotManager: Failed to get lot from database {}", static_cast<int>(ret));
		return false;
	}
	return VerifyLot(lot);
}

bool LotManager::GetStationImage(
	const NRSC5::StationInfo &station,
	NRSC5::Lot &lot) const
{
	lot.component.mime = NRSC5_MIME_STATION_LOGO;
	lot.component.channel = NRSC5::FriendlyProgramId(station.current_program);

	const UTILS::StatusCodes ret = db_.GetLotSpecial(station, lot);
	if (ret == UTILS::StatusCodes::Empty)
		return false;
	if (ret != UTILS::StatusCodes::Ok)
	{
		Logger::Log(err, "LotManager: Failed to get station logo from database {}", static_cast<int>(ret));
		return false;
	}
	return VerifyLot(lot);
}

void LotManager::LotReceived(
	const NRSC5::StationInfo &station,
	const NRSC5::Lot &lot) const
{
	// Check if we already have this lot
	if (IsLotAlreadyStored(station, lot))
	{
		Logger::Log(debug, "LotManager: We already have this lot {}", lot.id);
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

	if (db_.InsertLot(station, lot, path) == UTILS::StatusCodes::Ok)
	{
		Logger::Log(info, "LotManager: Saved lot {}", lot.name);
	}
	else
	{
		Logger::Log(err, "LotManager: Failed to save lot {} to database", lot.name);
		// Delete the lot file if we failed to save it to the database
		PhysicallyDelete(path);
	}
}

bool LotManager::VerifyLot(const NRSC5::Lot &lot) const
{
	if (!exists(lot.path))
	{
		Logger::Log(warn,
					"LotManager: Tried to access {}. Deleted file {}",
					lot.id,
					lot.path.string());
		// Clean up the database of that file if it's missing from the disk
		//SQLDeleteLotsFromFile(station, lot.path, false);
		return false;
	}

	if (!is_regular_file(lot.path))
	{
		Logger::Log(warn,
					"LotManager: Tried to access {}. Not a file {}",
					lot.id,
					lot.path.string());
		// Clean up the database of that file if it's not a file
		// SQLDeleteLotsFromFile(station, lot.path, false);
		PhysicallyDelete(lot.path);
		return false;
	}

	if (lot.isExpired())
	{
		Logger::Log(warn,
					"LotManager: Tried to access {}. Expired lot {}",
					lot.id,
					lot.path.string());
		// SQLDeleteLotsFromFile(station, lot.path, true);
		return false;
	}
	return true;
}

bool LotManager::IsLotAlreadyStored(
	const NRSC5::StationInfo &station,
	const NRSC5::Lot &lot) const
{
	NRSC5::Lot oldLot;
	oldLot.id = lot.id;

	return db_.GetLot(station, oldLot) == UTILS::StatusCodes::Ok
			&& oldLot.expire_point == lot.expire_point
			&& oldLot.mime == lot.mime;
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
// void LotManager::SQLDeleteLotsFromFile(const NRSC5::Station &station, const std::filesystem::path &path,
//                                        const bool expire_only) const
// {
// 	// // Check if we have any more lots with the same path
// 	// std::vector<NRSC5::Lot> lots;
// 	// SQLGetFile(station, path, lots);
// 	//
// 	// connection_manager_->Transaction([this, lots, station, expire_only](QSqlDatabase &)
// 	// {
// 	// 	for (const auto &lot : lots)
// 	// 	{
// 	// 		if (expire_only && !lot.isExpired())
// 	// 		{
// 	// 			continue;
// 	// 		}
// 	//
// 	// 		SQLDeleteLot(station, lot.id);
// 	// 	}
// 	// 	return true;
// 	// });
// }

int LotManager::SaveDisk(
	const std::filesystem::path &path, const vector_uint8_t &vector)
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
