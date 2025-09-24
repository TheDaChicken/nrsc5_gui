//
// Created by TheDaChicken on 8/20/2024.
//

#ifndef NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
#define NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_

#include "nrsc5/Station.h"
#include "RadioChannel.h"
#include "sql/DatabaseManager.h"
#include "sql/LotTable.h"

enum LotErr
{
	Lot_Success,
	Lot_DiskError,
	Lot_DBError,
	Lot_AlreadyExists,
	Lot_InvalidData,
	Lot_NotFound,
};

/**
 * Saves received lots to disk & stores data in database.
 */
class LotManager
{
	public:
		explicit LotManager(DatabaseManager &db_manager);

		void SetImageFolder(const std::filesystem::path &imagePath);

		tl::expected<void, LotErr> LotReceived(
			const StationIdentity &identity, const NRSC5::Lot &lot) const;

	private:
		static LotRecord FromLot(const Station &station, const NRSC5::Lot &lot);
		static tl::expected<void, LotErr> SaveDisk(const std::filesystem::path &path, const std::string &data);

		static bool IsSameLot(const LotRecord &oldLot, const LotRecord &newLot);

		[[nodiscard]] std::filesystem::path GetStationFolder(const StationIdentity &station) const;
		[[nodiscard]] tl::expected<std::filesystem::path, LotErr> EnsureStationFolder(const StationIdentity &station) const;

		std::filesystem::path image_path_;
		DatabaseManager &db_manager_;
};

#endif //NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
