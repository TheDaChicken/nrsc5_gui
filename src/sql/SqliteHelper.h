//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef SQLITEHELPER_H
#define SQLITEHELPER_H

#include "utils/Log.h"

#define SQLITE_RETURN_ERRSTR(func)                    \
ret = func;                                           \
if (ret != SQLITE_OK)                                 \
{                                                     \
Logger::Log(err, "SQLite: {}:{} {}", __FUNCTION__, __LINE__, sqlite3_errstr(ret));  \
return SqliteError(ret);                              \
}

#define SQLITE_RETURN_ERRMSG(func, message)                       \
do {                                                              \
ret = func;                                                       \
if (ret != SQLITE_OK)                                             \
{                                                                 \
	Logger::Log(err, "SQLite: {}: {}", message, sqlite3_errmsg(db));  \
	return SqliteError(ret);                                       \
}                                                                  \
} while (0)
inline UTILS::StatusCodes SqliteError(int ret)
{
	switch (ret)
	{
		case SQLITE_OK:
			return UTILS::StatusCodes::Ok;
		case SQLITE_BUSY:
			return UTILS::StatusCodes::SQLBusy;
		case SQLITE_ERROR:
		default:
			return UTILS::StatusCodes::DatabaseError;
	}
}

#endif //SQLITEHELPER_H
