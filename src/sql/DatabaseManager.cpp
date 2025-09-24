//
// Created by TheDaChicken on 8/27/2025.
//

#include "DatabaseManager.h"

#include "SchemaTable.h"

tl::expected<void, SQLiteError> DatabaseManager::Open(
	const std::filesystem::path &path,
	const int pool_num)
{
	auto pool_ret = pool_.Open(path, pool_num);
	if (!pool_ret)
	{
		Logger::Log(err, "SQLite: Unable to open database");
		return tl::unexpected(pool_ret.error());
	}

	auto connection = PopConnection();
	if (!connection)
	{
		Logger::Log(err, "SQLite: Unable to get database connection");
		return tl::unexpected(connection.error());
	}

	SchemaTable schema(*connection);

	auto table_result = schema.CreateSchemaTable();
	if (!table_result)
		return tl::unexpected(table_result.error());

	auto version_ref = schema.GetSchemaVersion();
	if (!version_ref)
	{
		if (version_ref.error() != Lite_Done)
		{
			Logger::Log(err, "SQLite: Unable to get schema version");
			return tl::unexpected(version_ref.error());
		}

		auto schema_result = schema.CreateSchema();
		if (!schema_result)
			return tl::unexpected(schema_result.error());

		return {};
	}

	int version = version_ref.value();
	int supported_schema = schema.GetSupportedSchema();

	if (supported_schema != version)
	{
		Logger::Log(err,
					"SQLite: Schema version mismatch: {} != {}",
					supported_schema,
					version);
		return tl::unexpected(Lite_Error);
	}
	return {};
}

tl::expected<std::shared_ptr<SQLite::Connection>, SQLiteError> DatabaseManager::PopConnection()
{
	return pool_.Pop();
}
