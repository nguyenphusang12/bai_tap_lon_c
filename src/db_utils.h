#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <sqlite3.h>
#include <string>

bool initDatabase(sqlite3** db);
bool executeSQL(sqlite3* db, const std::string& sql);
bool userQuery(sqlite3* db, const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

#endif