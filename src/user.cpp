#include <string>
#include <sqlite3.h>
#include "db_utils.h"

int getUserIdByUsername(const std::string& username) {
    sqlite3* db;
    if (!initDatabase(&db)) return -1;

    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE username = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int userId = -1;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return userId;
}
