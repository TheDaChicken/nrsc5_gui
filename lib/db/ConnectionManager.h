//
// Created by TheDaChicken on 8/20/2024.
//

#ifndef NRSC5_GUI_LIB_DB_CONNECTIONMANAGER_H_
#define NRSC5_GUI_LIB_DB_CONNECTIONMANAGER_H_

#include <QSqlDatabase>
#include <QFuture>
#include <QCache>
#include <QThreadStorage>

/**
 * @brief Handles pool of database connections for the application
 * Warning: You must use QT threading to use this class
 * FIXME: Can we slowly move away from using QT SQL
 * (I think it's stupid to use QT SQL when we can use SQLite directly)
 */
class ConnectionManager
{
 public:
  ConnectionManager();

  /**
   * @brief Get a connection to the database for the current thread
   * @return The connection to the database
   */
  [[nodiscard]] QSqlDatabase GetConnection() const;

  /**
   * @brief Get prepared query for the current thread or create a new one
   * Since prepared queries are connection specific,
   * This function will return a prepared query for the current thread
   */
  QSqlQuery *GetPreparedQuery(const QString &query);

  /**
   * @brief Set the database name to use for the connection
   * @param databaseName The name of the database to use
   */
  void SetDatabaseName(const QString &databaseName);

  /**
   * @brief Commit or rollback on error the current transaction
   * Connection based on the current thread
   */
  bool Transaction(const std::function<bool(QSqlDatabase &)> &transaction) const;

  void DebugLogging();

  [[nodiscard]] QString GetSQLiteVersion() const;

 private:
  static QString GetThreadName();

  QThreadStorage<QCache<QString, QSqlQuery> > prepared_queries_;

  void CreateConnection() const;
  QSqlQuery *CreatePreparedQuery(const QString &threadName, const QString &query);

  QString database_name_;
};

#endif //NRSC5_GUI_LIB_DB_CONNECTIONMANAGER_H_
