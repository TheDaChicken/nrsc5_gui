//
// Created by TheDaChicken on 2/16/2025.
//

#ifndef SQLITEHELPER_H
#define SQLITEHELPER_H

#include "../sqlite/sqlite3.h"

enum SQLiteError : int
{
	Lite_Ok = SQLITE_OK,
	Lite_Error = SQLITE_ERROR,
	Lite_Internal = SQLITE_INTERNAL,
	Lite_Perm = SQLITE_PERM,
	Lite_Abort = SQLITE_ABORT,
	Lite_Busy = SQLITE_BUSY,
	Lite_Locked = SQLITE_LOCKED,
	Lite_NoMem = SQLITE_NOMEM,
	Lite_ReadOnly = SQLITE_READONLY,
	Lite_Interrupt = SQLITE_INTERRUPT,
	Lite_IoErr = SQLITE_IOERR,
	Lite_Corrupt = SQLITE_CORRUPT,
	Lite_NotFound = SQLITE_NOTFOUND,
	Lite_Full = SQLITE_FULL,
	Lite_CantOpen = SQLITE_CANTOPEN,
	Lite_Protocol = SQLITE_PROTOCOL,
	Lite_Empty = SQLITE_EMPTY,
	Lite_Schema = SQLITE_SCHEMA,
	Lite_TooBig = SQLITE_TOOBIG,
	Lite_Constraint = SQLITE_CONSTRAINT,
	Lite_Mismatch = SQLITE_MISMATCH,
	Lite_Misuse = SQLITE_MISUSE,
	Lite_NoLfs = SQLITE_NOLFS,
	Lite_Auth = SQLITE_AUTH,
	Lite_Format = SQLITE_FORMAT,
	Lite_Range = SQLITE_RANGE,
	Lite_NotADb = SQLITE_NOTADB,
	Lite_Notice = SQLITE_NOTICE,
	Lite_Warning = SQLITE_WARNING,
	Lite_Row = SQLITE_ROW,
	Lite_Done = SQLITE_DONE
};


#endif //SQLITEHELPER_H
