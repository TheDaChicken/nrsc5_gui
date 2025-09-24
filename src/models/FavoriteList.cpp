//
// Created by TheDaChicken on 7/22/2025.
//

#include "FavoriteList.h"

constexpr auto kSettingKey = "favorite_channels";

FavoriteList::FavoriteList(
	DatabaseManager &db_manager)
	: db_manager_(db_manager)
{
}

bool FavoriteList::Update()
{
	bool result = true;

	auto pool = db_manager_.PopConnection();
	if (!pool)
		return false;

	SettingsTable table(*pool);

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
	// nlohmann::json channels = nlohmann::json::array();
	// for (const auto &item : channels_)
	// {
	// 	channels.push_back(item.channel);
	// }
	//
	// const std::string value = channels.dump();
	//
	// if (SqliteError ret = db_.SetSettingValue(kSettingKey, value);
	// 	ret != SqliteError::Ok)
	// {
	// 	Logger::Log(err, "Failed to save favorite channels: {}", static_cast<int>(ret));
	// 	return false;
	// }

	return true;
}
