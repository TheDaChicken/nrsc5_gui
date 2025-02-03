//
// Created by TheDaChicken on 8/20/2024.
//

#ifndef NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
#define NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_

#include <QString>

#include "Station.h"
#include "db/ConnectionManager.h"
#include "RadioChannel.h"

/**
 * Saves received lots to disk & stores data in database.
 */
class LotManager
{
	public:
		explicit LotManager(ConnectionManager *connection_manager);

		int Open();
		int SetImageFolder(const std::filesystem::path &imagePath);

		bool SQLGetStationImage(const NRSC5::Station &station, NRSC5::Lot &lot) const;
		bool GetLot(const NRSC5::Station &station, NRSC5::Lot &lot) const;

		void LotReceived(const NRSC5::Station &station,
		                 const NRSC5::DataService &component, const NRSC5::Lot &lot) const;

	private:
		void SQLDeleteLotsFromFile(const NRSC5::Station &station, const std::filesystem::path &path,
		                        bool expire_only) const;

		bool SaveSpecialMIME(const NRSC5::Station &station,
		                     const NRSC5::DataService &component,
		                     const NRSC5::Lot &lot) const;
		bool IsLotAlreadyStored(const NRSC5::Station &station, const NRSC5::Lot &lot) const;

		void SQLDeleteLot(const NRSC5::Station &station, unsigned int lot) const;
		bool SQLInsertLot(const NRSC5::Station &station,
		                  const NRSC5::DataService &component,
		                  const NRSC5::Lot &lot, const std::filesystem::path &path) const;
		bool SQLInsertStationLogo(const NRSC5::Station &station, const NRSC5::Lot &lot,
		                          unsigned int programId) const;
		bool SQLGetLot(const NRSC5::Station &station, NRSC5::Lot &lot) const;
		void SQLGetFile(const NRSC5::Station &station, const std::filesystem::path &path,
		                std::vector<NRSC5::Lot> &lots) const;

		static int SaveDisk(const std::filesystem::path &path, const vector_uint8_t &vector);
		static void PhysicallyDelete(const std::filesystem::path &path);
		static void SQLtoLot(const QSqlQuery &query, NRSC5::Lot &lot);

		std::filesystem::path image_path_;
		std::filesystem::path StationFolder(const NRSC5::Station &station) const;

		ConnectionManager *connection_manager_;
};

#endif //NRSC5_GUI_LIB_NRSC5_LOTMANAGER_H_
