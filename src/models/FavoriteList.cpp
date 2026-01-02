//
// Created by TheDaChicken on 7/22/2025.
//

#include "FavoriteList.h"

#include "sql/SettingsTable.h"

constexpr auto kSettingKey = "favorite_channels";

FavoriteList::FavoriteList(
	DatabaseManager &db_manager)
	: db_manager_(db_manager)
{
}

bool FavoriteList::Update()
{
	bool result = true;

	auto conn = db_manager_.PopConnection();
	if (!conn)
	{
		Logger::Log(err, "Failed to pop a db connection");
		return false;
	}

	SettingsTable table(*conn);

	auto value = table.GetSettingValue(kSettingKey);
	if (!value)
	{
		Logger::Log(err, "Failed to get favorite channels from db");
		return result;
	}

	try
	{
		m_favorites = nlohmann::json::parse(value.value());
	} catch (const std::exception &e)
	{
		Logger::Log(err, "Failed to parse favorite channels: {}", e.what());
		result = false;
	}

	if (result && on_update)
		on_update();

	return result;
}

bool FavoriteList::Submit()
{
	// TODO: Implement saving favorites back to database
	return true;
}
