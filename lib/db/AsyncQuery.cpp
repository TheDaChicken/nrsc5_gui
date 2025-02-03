//
// Created by TheDaChicken on 8/20/2024.
//

#include "AsyncQuery.h"
#include "ConnectionManager.h"

#include <QtConcurrent/QtConcurrent>

AsyncQuery::AsyncQuery(ConnectionManager &connection_manager)
	: connection_manager_(connection_manager)
{
}

void AsyncQuery::Prepare(const QString &query)
{
  query_string_ = query;
}

QFuture<bool> AsyncQuery::ExecuteShootBind(QMap<QString, QVariant> &bind_values)
{
  return QtConcurrent::run([this, bind_values]()
  {
	QSqlQuery* query = connection_manager_.GetPreparedQuery(query_string_);

	for (auto it = bind_values.begin(); it != bind_values.end(); ++it)
	{
	  query->bindValue(it.key(), it.value());
	}

	// Execute the query
	query->exec();
	return true;
  });
}
