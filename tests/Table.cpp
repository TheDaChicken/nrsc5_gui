//
// Created by TheDaChicken on 8/25/2025.
//

#include "sql/Table.h"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include "sql/DatabaseManager.h"

TEST(ConnectionPool, Init)
{
	SQLite::ConnectionPool pool;
	EXPECT_TRUE(pool.Open(":memory:", 2)) << "Failed to initialize connection pool";
}

TEST(ConnectPool, PopConnection)
{
	SQLite::ConnectionPool pool;
	EXPECT_TRUE(pool.Open(":memory:", 1)) << "Failed to initialize connection pool";

	{
		const auto conn1 = pool.Pop();
		EXPECT_TRUE(conn1) << "Failed to get first connection";

		const auto conn2 = pool.Pop();
		EXPECT_FALSE(conn2) << "Should not have been able to get second connection";
	}

	const auto conn2 = pool.Pop();
	EXPECT_TRUE(conn2) << "Failed to get connection after first was returned";
}

TEST(ConnectPool, TestStatement)
{
	SQLite::ConnectionPool pool;
	EXPECT_TRUE(pool.Open(":memory:", 1)) << "Failed to initialize connection pool";

	auto conn = pool.Pop();
	EXPECT_TRUE(conn) << "Failed to get connection";
	const auto &db = conn.value();
	auto statement_ref = db->Prepare("SELECT 1;");
	EXPECT_TRUE(statement_ref) << "Failed to prepare statement";
	const auto &statement = *statement_ref;
	EXPECT_EQ(statement->Step(), SQLITE_ROW) << "Failed to step statement";
	EXPECT_EQ(statement->GetColumnInt(0), 1) << "Failed to read integer from";
	EXPECT_EQ(statement->Step(), SQLITE_DONE) << "Statement should be done";
}

TEST(Database, Init)
{
}

TEST(Database, Schema)
{
	DatabaseManager pool;
	EXPECT_TRUE(pool.Open(":memory:", 1)) << "Failed to initialize connection pool";

	auto db = pool.AcquireSchema();
	ASSERT_TRUE(db) << "Failed to get connection";

	EXPECT_THAT(db->GetSchemaVersion(),
	            testing::Optional(testing::Eq(db->GetSupportedSchema()))) << "Failed to get schema version";
	EXPECT_TRUE(db->SetSchemaVersion(5));
	EXPECT_THAT(db->GetSchemaVersion(), testing::Optional(testing::Eq(5))) << "Failed to get new schema version";
}

TEST(Database, Settings)
{
	DatabaseManager pool;
	EXPECT_TRUE(pool.Open(":memory:", 1)) << "Failed to initialize connection pool";

	auto db = pool.AcquireSettings();
	ASSERT_TRUE(db) << "Failed to get connection";

	EXPECT_EQ(db->GetSettingValue("nonexistent"), tl::unexpected(Lite_Done));

	EXPECT_TRUE(db->SetSettingValue("key1", "value1"));
	EXPECT_THAT(db->GetSettingValue("key1"), testing::Optional(testing::Eq("value1")));

	EXPECT_TRUE(db->SetSettingValue("key1", "value2"));
	EXPECT_THAT(db->GetSettingValue("key1"), testing::Optional(testing::Eq("value2")));

	EXPECT_TRUE(db->SetSettingValue("key2", "value3"));
	EXPECT_THAT(db->GetSettingValue("key2"), testing::Optional(testing::Eq("value3")));
}

TEST(Database, InsertAndGetLot)
{
	DatabaseManager pool;
	EXPECT_TRUE(pool.Open(":memory:", 1)) << "Failed to initialize connection pool";

	auto db = pool.AcquireLot();
	ASSERT_TRUE(db) << "Failed to get connection";

	LotRecord lot;
	lot.callsign = "TEST";
	lot.channel = 5;
	lot.service = NRSC5_MIME_PNG;
	lot.id = 1;
	lot.mime = NRSC5_MIME_JPEG;
	lot.path = "/path/to/lot";
	lot.expire_point = std::chrono::system_clock::now() + std::chrono::hours(1);

	EXPECT_TRUE(db->InsertLot(lot));

	LotRecord key;
	key.callsign = lot.callsign;
	key.channel = lot.channel;
	key.service = lot.service;
	key.id = lot.id;

	auto fetched_lot_ref = db->GetLot(key);
	ASSERT_TRUE(fetched_lot_ref) << "Failed to get inserted lot";

	const auto &fetched_lot = fetched_lot_ref.value();

	EXPECT_EQ(fetched_lot.id, lot.id);
	EXPECT_EQ(fetched_lot.mime, lot.mime);
	EXPECT_EQ(fetched_lot.path, "/path/to/lot");
}
