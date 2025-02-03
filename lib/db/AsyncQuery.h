//
// Created by TheDaChicken on 8/20/2024.
//

#ifndef NRSC5_GUI_LIB_DB_ASYNCQUERY_H_
#define NRSC5_GUI_LIB_DB_ASYNCQUERY_H_

#include <QString>
#include <QFuture>
#include <QSqlQuery>
#include "ConnectionManager.h"

/**
 * @brief Asynchronous query class
 */
class AsyncQuery
{
 public:
  explicit AsyncQuery(ConnectionManager &connection_manager);

  void Prepare(const QString &query);
  QFuture<bool> ExecuteShootBind(QMap<QString, QVariant> &bind_values);
  QFuture<bool> ExecuteShoot()
  {
	QMap<QString, QVariant> bind_values;
	return ExecuteShootBind(bind_values);
  }
 private:
  ConnectionManager &connection_manager_;
  QString query_string_;
};

#endif //NRSC5_GUI_LIB_DB_ASYNCQUERY_H_
