//
// Created by TheDaChicken on 8/29/2025.
//

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <memory>

#include "connection/Connection.h"

class SettingsTable final
{
	public:
		explicit SettingsTable(std::shared_ptr<SQLite::Connection> conn)
			: conn_(std::move(conn))
		{
		}
		~SettingsTable() = default;

		tl::expected<std::string, SQLiteError> GetSettingValue(std::string_view key);
		tl::expected<void, SQLiteError> SetSettingValue(std::string_view key, std::string_view value);

	private:
		std::shared_ptr<SQLite::Connection> conn_;
};

#endif //SETTINGSMANAGER_H
