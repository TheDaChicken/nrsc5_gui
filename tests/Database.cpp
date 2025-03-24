//
// Created by TheDaChicken on 7/23/2024.
//
#include <gtest/gtest.h>

#include "sql/Database.h"

TEST(Database, Open)
{
	SQLite::Database db;
	ASSERT_EQ(db.Open(":memory:"), UTILS::StatusCodes::Ok);
}

TEST(Database, Settings)
{
	SQLite::Database db;
	ASSERT_EQ(db.Open(":memory:"), UTILS::StatusCodes::Ok);

	const std::string key = "key";
	const std::string value = "value";
	db.SetSettingValue(key, value);

	std::string result;
	ASSERT_EQ(db.GetSettingValue(key, result), UTILS::StatusCodes::Ok);
	ASSERT_EQ(result, value);
}

TEST(Database, LOT)
{
	SQLite::Database db;
	ASSERT_EQ(db.Open(":memory:"), UTILS::StatusCodes::Ok);

	NRSC5::StationInfo mock_station;
	mock_station.id = 21;
	mock_station.name = "Mock Station";

	NRSC5::DataService mock_component;
	mock_component.channel = 1;
	mock_component.mime = NRSC5_MIME_PRIMARY_IMAGE;

	NRSC5::Lot lot;
	lot.id = 100; // Unique per component per station
	lot.component = mock_component;
	lot.mime = NRSC5_MIME_JPEG;
	lot.path = "test.jpg";
	lot.expire_point = std::chrono::system_clock::now() + std::chrono::hours(1);

	ASSERT_EQ(db.InsertLot(mock_station, lot, lot.path), UTILS::StatusCodes::Ok);

	NRSC5::Lot result;
	result.id = lot.id;
	result.component = mock_component;

	ASSERT_EQ(db.GetLot(mock_station, result), UTILS::StatusCodes::Ok);
	ASSERT_EQ(lot.mime, result.mime);
	ASSERT_EQ(lot.path, result.path);
	// Database only stores up to seconds
	ASSERT_EQ(std::chrono::duration_cast<std::chrono::seconds>(lot.expire_point.time_since_epoch()), result.expire_point.time_since_epoch());

	ASSERT_EQ(db.DeleteLot(mock_station, result), UTILS::StatusCodes::Ok);
	ASSERT_EQ(db.GetLot(mock_station, result), UTILS::StatusCodes::Empty);
}
