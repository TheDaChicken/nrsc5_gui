//
// Created by TheDaChicken on 8/20/2024.
//

#include "ConnectionManager.h"
#include "utils/Log.h"

#include <QThread>
#include <QSqlError>
#include <QSqlQuery>

ConnectionManager::ConnectionManager() = default;

QSqlDatabase ConnectionManager::GetConnection() const
{
  const QString kGetThreadName = GetThreadName();
  if (!QSqlDatabase::contains(kGetThreadName))
  {
    CreateConnection();
  }
  return QSqlDatabase::database(kGetThreadName);
}

QSqlQuery *ConnectionManager::GetPreparedQuery(const QString &query)
{
  const QString thread_name = GetThreadName() + "_" + query;

  QSqlQuery *prepared_query = prepared_queries_.localData().object(thread_name);
  if (prepared_query == nullptr)
  {
    prepared_query = CreatePreparedQuery(thread_name, query);
  }

  return prepared_query;
}

QSqlQuery *ConnectionManager::CreatePreparedQuery(const QString &threadName, const QString &query)
{
  const auto prepared_query = new QSqlQuery(GetConnection());

  if (!prepared_query->prepare(query))
  {
    Logger::Log(err, "Failed to prepare query: {}", prepared_query->lastError().text());
    delete prepared_query;
    return nullptr;
  }

  prepared_queries_.localData().insert(threadName, prepared_query, 1);
  return prepared_query;
}

void ConnectionManager::CreateConnection() const
{
  const QString kGetThreadName = GetThreadName();

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kGetThreadName);
  db.setDatabaseName(database_name_);
  db.open();
}

QString ConnectionManager::GetSQLiteVersion() const
{
  const QSqlDatabase db = GetConnection();
  QSqlQuery q(db);

  q.exec("select sqlite_version();");
  if (!q.next())
  {
    return {};
  }

  return q.value(0).toString();
}

void ConnectionManager::SetDatabaseName(const QString &databaseName)
{
  database_name_ = databaseName;
}

QString ConnectionManager::GetThreadName()
{
  return "pool_" + QString::number(reinterpret_cast<quint64>(QThread::currentThread()), 16);
}

void ConnectionManager::DebugLogging()
{
  Logger::Log(debug, "Database Name: {}", database_name_);
  Logger::Log(debug, "SQL Version: {}", GetSQLiteVersion());
}

bool ConnectionManager::Transaction(const std::function<bool(QSqlDatabase &)> &transaction) const
{
  QSqlDatabase db = GetConnection();

  db.transaction();

  // If there is an error, rollback the transaction
  if (!transaction(db) && db.lastError().isValid())
  {
    db.rollback();
    return false;
  }

  return db.commit();
}

