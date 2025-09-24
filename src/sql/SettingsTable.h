//
// Created by TheDaChicken on 8/29/2025.
//

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "Table.h"

class SettingsTable final : public Table
{
	public:
		explicit SettingsTable(std::shared_ptr<SQLite::Connection> conn)
			: Table(std::move(conn))
		{
		}
		~SettingsTable() override = default;

		tl::expected<std::string, SQLiteError> GetSettingValue(std::string_view key);
		tl::expected<void, SQLiteError> SetSettingValue(std::string_view key, std::string_view value);
};

#endif //SETTINGSMANAGER_H
