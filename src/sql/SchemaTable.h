//
// Created by TheDaChicken on 8/29/2025.
//

#ifndef SCHEMAMANAGER_H
#define SCHEMAMANAGER_H

#include "Table.h"

class SchemaTable final : public Table
{
	public:
		explicit SchemaTable(std::shared_ptr<SQLite::Connection> db) : Table(std::move(db)) {}
		~SchemaTable() override = default;

		tl::expected<void, SQLiteError> CreateSchemaTable();
		tl::expected<void, SQLiteError> CreateSchema();

		tl::expected<int, SQLiteError> GetSchemaVersion();
		tl::expected<void, SQLiteError> SetSchemaVersion(int version);

		int GetSupportedSchema() const;
};

#endif //SCHEMAMANAGER_H
