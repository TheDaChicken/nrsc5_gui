//
// Created by TheDaChicken on 8/29/2025.
//

#ifndef LOTMANAGER_H
#define LOTMANAGER_H

#include "Table.h"

struct LotRecord
{
	std::string callsign;
	unsigned int channel;
	uint32_t service;

	unsigned int id;
	uint32_t mime;
	std::filesystem::path path;
	std::chrono::system_clock::time_point expire_point;
};

class LotTable final : public Table
{
	public:
		explicit LotTable(std::shared_ptr<SQLite::Connection> db) : Table(std::move(db))
		{
		}
		~LotTable() override = default;

		tl::expected<void, SQLiteError> InsertLot(const LotRecord &key);
		tl::expected<void, SQLiteError> DeleteLot(const LotRecord &key);

		tl::expected<LotRecord, SQLiteError> GetLot(const LotRecord &key) const;
		tl::expected<LotRecord, SQLiteError> GetLotSpecial(const LotRecord &key) const;

	private:
		static void ConvertToLot(const SQLite::StatementHandle &stmt, LotRecord &lot);
};

#endif //LOTMANAGER_H
