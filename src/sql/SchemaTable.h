//
// Created by TheDaChicken on 8/29/2025.
//

#ifndef SCHEMAMANAGER_H
#define SCHEMAMANAGER_H

#include <memory>

#include "connection/Connection.h"

class SchemaTable
{
	public:
		explicit SchemaTable(std::shared_ptr<SQLite::Connection> db) : conn_(std::move(db)) {}
		~SchemaTable() = default;

		tl::expected<void, SQLiteError> CreateSchemaTable();
		tl::expected<void, SQLiteError> CreateSchema();

		tl::expected<int, SQLiteError> GetSchemaVersion();
		tl::expected<void, SQLiteError> SetSchemaVersion(int version);

		int GetSupportedSchema() const;
	private:
		std::shared_ptr<SQLite::Connection> conn_;
};

#endif //SCHEMAMANAGER_H
