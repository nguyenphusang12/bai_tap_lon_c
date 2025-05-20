#include "db_utils.h"
#include <iostream>
#include <sqlite3.h>
#include "auth.h"
#include "wallet.h"

bool initDatabase(sqlite3** db) {
    int rc = sqlite3_open("db/reward_wallet.db", db);
    if (rc) {
        std::cerr << "Cannot open DB: " << sqlite3_errmsg(*db) << "\n";
        return false;
    }

    std::string createUserTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE,
            password TEXT,
            fullName TEXT,
            email TEXT,
            role INTEGER,
            requirePasswordChange INTEGER
        );
    )";

    std::string createWalletTable = R"(
        CREATE TABLE IF NOT EXISTS wallets (
            walletId TEXT PRIMARY KEY,
            userId INTEGER,
            balance INTEGER,
            FOREIGN KEY(userId) REFERENCES users(id)
        );
    )";

    std::string createTransactionTable = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            fromWallet TEXT,
            toWallet TEXT,
            amount INTEGER,
            status TEXT,
            timestamp TEXT
        );
    )";

    bool tablesCreated = executeSQL(*db, createUserTable) &&
                         executeSQL(*db, createWalletTable) &&
                         executeSQL(*db, createTransactionTable);

    if (!tablesCreated) return false;

    sqlite3_stmt* stmt;
    const char* checkSql = "SELECT id FROM users WHERE username = 'admin';";
    if (sqlite3_prepare_v2(*db, checkSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Lỗi prepare kiểm tra admin.\n";
        return false;
    }

    bool adminExists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        adminExists = true;
    }
    sqlite3_finalize(stmt);

    if (!adminExists) {
        std::string hashed = hashPassword("admin");
        const char* insertSql = R"(
            INSERT INTO users (username, password, fullName, email, role, requirePasswordChange)
            VALUES (?, ?, 'Admin Default', 'admin@example.com', ?, 1);
        )";
    
        if (sqlite3_prepare_v2(*db, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Lỗi prepare insert admin.\n";
            return false;
        }
    
        sqlite3_bind_text(stmt, 1, "admin", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, ADMIN);
    
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Lỗi thêm tài khoản admin mặc định.\n";
            sqlite3_finalize(stmt);
            return false;
        }
    
        sqlite3_finalize(stmt);
    
        int adminId = sqlite3_last_insert_rowid(*db);
        createWalletForUser(adminId);
    
        std::cout << "Đã tạo tài khoản admin mặc định (admin/admin).\n";
    }

    return true;
}

bool executeSQL(sqlite3* db, const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool userQuery(sqlite3* db, const std::string& sql, int (*callback)(void*, int, char**, char**), void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
