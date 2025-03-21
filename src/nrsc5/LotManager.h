//
// Created by TheDaChicken on 8/20/2024.
//

#ifndef NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
#define NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_

#include "../sql/Database.h"
#include "nrsc5/Station.h"
#include "RadioChannel.h"

/**
 * Saves received lots to disk & stores data in database.
 */
class LotManager
{
	public:
		explicit LotManager(SQLite::Database &database_manager);

		int SetImageFolder(const std::filesystem::path &imagePath);

		bool GetLot(const NRSC5::StationInfo &station, const NRSC5::DataService &component, NRSC5::Lot &lot) const;
		bool GetStationImage(const NRSC5::StationInfo &station, NRSC5::Lot &lot) const;

		void LotReceived(const NRSC5::StationInfo &station, const NRSC5::DataService &component,
		                 const NRSC5::Lot &lot) const;

	private:
		bool VerifyLot(const NRSC5::Lot &lot) const;
		bool IsLotAlreadyStored(const NRSC5::StationInfo &station, const NRSC5::DataService &component,
		                        const NRSC5::Lot &lot) const;

		static int SaveDisk(const std::filesystem::path &path, const vector_uint8_t &vector);
		static void PhysicallyDelete(const std::filesystem::path &path);

		std::filesystem::path image_path_;
		[[nodiscard]] std::filesystem::path StationFolder(const NRSC5::StationInfo &station) const;

		SQLite::Database &db_;
};

#endif //NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
