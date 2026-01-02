//
// Created by TheDaChicken on 8/29/2025.
//

#include "LotTable.h"

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

static constexpr auto kGetSpecialHDRadioLot = R"(
	SELECT * FROM HDRADIO_LOTS
	WHERE
		callSign = :callSign AND
		channel = :channel AND
		service = :service
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

tl::expected<void, SQLiteError> LotTable::InsertLot(
	const LotRecord &key)
{
	assert(key.id > 0);
	assert(key.channel > 0);

	const auto expire = std::chrono::duration_cast<std::chrono::seconds>(
		key.expire_point.time_since_epoch()).count();
	const auto str = key.path.string(); // Keep this in scope to avoid dangling pointer in sqlite

	return conn_->InsertData(
		kInsertHDRadioLot,
		std::make_pair(":callSign", key.callsign),
		std::make_pair(":channel", key.channel),
		std::make_pair(":service", key.service),
		std::make_pair(":lotId", key.id),
		std::make_pair(":mime", key.mime),
		std::make_pair(":path", str),
		std::make_pair(":expire", expire)
	);
}

tl::expected<void, SQLiteError> LotTable::DeleteLot(
	const LotRecord &key)
{
	return conn_->InsertData(
		kDeleteHDRadioLot,
		std::make_pair(":callSign", key.callsign),
		std::make_pair(":channel", key.channel),
		std::make_pair(":service", key.service),
		std::make_pair(":lotId", key.id)
	);
}

tl::expected<LotRecord, SQLiteError> LotTable::GetLot(
	const LotRecord &key) const
{
	assert(key.id > 0);
	assert(key.channel > 0);
	assert(key.service > 0);

	return conn_->QueryData<LotRecord>(
		kGetHDRadioLot,
		[](const SQLite::StatementHandle &stmt, LotRecord &value)
		{
			ConvertToLot(stmt, value);
		},
		std::make_pair(":callSign", key.callsign),
		std::make_pair(":channel", key.channel),
		std::make_pair(":service", key.service),
		std::make_pair(":lotId", key.id)
	);
}

tl::expected<LotRecord, SQLiteError> LotTable::GetLotSpecial(
	const LotRecord &key) const
{
	assert(key.channel > 0);

	return conn_->QueryData<LotRecord>(
		kGetSpecialHDRadioLot,
		[](const SQLite::StatementHandle &stmt, LotRecord &value)
		{
			ConvertToLot(stmt, value);
		},
		std::make_pair(":callSign", key.callsign),
		std::make_pair(":channel", key.channel),
		std::make_pair(":service", key.service)
	);
}

void LotTable::ConvertToLot(
	const SQLite::StatementHandle &stmt,
	LotRecord &lot)
{
	lot.channel = stmt->GetColumnInt(1);
	lot.service = stmt->GetColumnInt(2);
	lot.id = stmt->GetColumnInt(3);
	lot.mime = stmt->GetColumnInt(4);
	lot.path = stmt->GetColumnString(5);
	lot.expire_point = std::chrono::system_clock::time_point(
		std::chrono::seconds{stmt->GetColumnInt64(6)});
}
