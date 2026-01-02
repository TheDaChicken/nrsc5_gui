//
// Created by TheDaChicken on 8/29/2025.
//

#include "SettingsTable.h"

static constexpr auto kInsertSetting = R"(
	INSERT OR REPLACE INTO SETTINGS (key, value) VALUES (:key, :value);
)";

static constexpr auto kGetSettingValue = R"(
	SELECT * FROM SETTINGS WHERE key = :key;
)";

tl::expected<std::string, SQLiteError> SettingsTable::GetSettingValue(
	const std::string_view key)
{
	return conn_->QueryData<std::string>(
		kGetSettingValue,
		[](const SQLite::StatementHandle &stmt, std::string &lot)
		{
			lot = stmt->GetColumnString(1);
		},
		std::make_pair(":key", key)
	);
}

tl::expected<void, SQLiteError> SettingsTable::SetSettingValue(
	const std::string_view key,
	const std::string_view value)
{
	return conn_->InsertData(
		kInsertSetting,
		std::make_pair(":key", key),
		std::make_pair(":value", value)
	);
}