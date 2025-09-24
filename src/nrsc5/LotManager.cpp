//
// Created by TheDaChicken on 8/20/2024.
//

#include "LotManager.h"

#include "utils/Log.h"

#include <fstream>

LotManager::LotManager(DatabaseManager &db_manager)
	: db_manager_(db_manager)
{
}

void LotManager::SetImageFolder(
	const std::filesystem::path &imagePath)
{
	image_path_ = imagePath;
}

tl::expected<void, LotErr> LotManager::LotReceived(
	const StationIdentity &identity,
	const NRSC5::Lot &lot) const
{
	auto conn = db_manager_.PopConnection();
	if (!conn)
	{
		Logger::Log(err,
		            "LotManager: Failed to get database connection {}",
		            static_cast<int>(conn.error()));
		return tl::unexpected(Lot_DBError);
	}

	LotTable db(conn.value());
	LotRecord lotRecord;
	lotRecord.callsign = identity.name;
	lotRecord.channel = lot.component.channel;
	lotRecord.service = lot.component.mime;
	lotRecord.id = lot.id;
	lotRecord.mime = lot.mime;
	lotRecord.expire_point = lot.expire_point;

	// Check if we already have this lot
	auto oldLot = db.GetLot(lotRecord);
	if (oldLot && IsSameLot(oldLot.value(), lotRecord))
	{
		Logger::Log(debug, "LotManager: We already have this lot {}. Skipping saving", lot.id);
		return {};
	}

	auto folder = EnsureStationFolder(identity);
	if (!folder)
	{
		Logger::Log(err,
		            "LotManager: Failed to ensure station folder for {}: {}",
		            identity.name,
		            static_cast<int>(folder.error()));
		return tl::unexpected(folder.error());
	}

	lotRecord.path = folder.value() / lot.name;

	if (!SaveDisk(lotRecord.path, lot.data))
	{
		Logger::Log(err, "LotManager: Failed to save lot onto disk {}", lotRecord.path.filename().string());
		return tl::unexpected(Lot_DiskError);
	}

	if (!db.InsertLot(lotRecord))
	{
		Logger::Log(err, "LotManager: Failed to save lot {} to database", lotRecord.path.filename().string());
		// Delete the file if we failed to insert into DB
		std::filesystem::remove(lotRecord.path);
		return tl::unexpected(Lot_DBError);
	}

	Logger::Log(info,
	            "LotManager: Successfully saved lot {} {}",
	            lot.id,
	            lot.name);
	return {};
}

tl::expected<void, LotErr> LotManager::SaveDisk(
	const std::filesystem::path &path,
	const std::string &data)
{
	std::ofstream image(path, std::ios_base::binary);
	if (image.fail())
	{
		Logger::Log(err,
		            "LotManager: Unable to create open stream for {}: {}",
		            path.string(),
		            strerror(errno));
		return tl::unexpected(Lot_DiskError);
	}

	image.write(data.c_str(), data.size());
	return {};
}

bool LotManager::IsSameLot(const LotRecord &oldLot, const LotRecord &newLot)
{
	if (!std::filesystem::exists(oldLot.path))
		return false;

	return oldLot.id == newLot.id &&
			oldLot.mime == newLot.mime &&
			oldLot.expire_point == newLot.expire_point;
}

std::filesystem::path LotManager::GetStationFolder(
	const StationIdentity &station) const
{
	return image_path_ / (station.country_code + "_" + std::to_string(station.id));
}

tl::expected<std::filesystem::path, LotErr> LotManager::EnsureStationFolder(const StationIdentity &station) const
{
	try
	{
		create_directory(image_path_);
	} catch (const std::filesystem::filesystem_error &e)
	{
		Logger::Log(err, "LotManager: Failed to create directory for images: {}", e.what());
		return tl::unexpected(Lot_DiskError);
	}

	const auto station_folder = GetStationFolder(station);

	// Create the station folder if it doesn't exist
	try
	{
		create_directory(station_folder);
	} catch (const std::exception &e)
	{
		Logger::Log(err,
					"LotManager: Failed to create station directory for {}: {}",
					station.name,
					e.what());
		return tl::unexpected(Lot_DiskError);
	}

	return station_folder;
}


