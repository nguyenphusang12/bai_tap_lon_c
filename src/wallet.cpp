#include "wallet.h"
#include "db_utils.h"
#include <iostream>
#include <sqlite3.h>

bool createWalletForUser(int userId) {
    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string walletId = "WALLET_" + std::to_string(userId);

    std::string checkSql = "SELECT COUNT(*) FROM wallets WHERE walletId = '" + walletId + "';";
    int count = 0;

    auto countCallback = [](void* data, int argc, char** argv, char**) -> int {
        int* count = static_cast<int*>(data);
        if (argc > 0 && argv[0]) *count = std::stoi(argv[0]);
        return 0;
    };

    userQuery(db, checkSql, countCallback, &count);

    if (count > 0) {
        sqlite3_close(db);
        return true;
    }
    
    std::string sql = "INSERT INTO wallets (walletId, userId, balance) VALUES ('" +
                      walletId + "', " + std::to_string(userId) + ", 10000);";
    bool ok = executeSQL(db, sql);
    sqlite3_close(db);
    return ok;
}

Wallet getWalletByUserId(int userId) {
    sqlite3* db;
    Wallet w = { "", userId, 0 };
    if (!initDatabase(&db)) return w;

    std::string sql = "SELECT walletId, balance FROM wallets WHERE userId=" + std::to_string(userId) + ";";

    auto callback = [](void* data, int, char** argv, char**) -> int {
        Wallet* w = static_cast<Wallet*>(data);
        w->walletId = argv[0];
        w->balance = std::stoi(argv[1]);
        return 0;
    };

    userQuery(db, sql, callback, &w);
    sqlite3_close(db);
    return w;
}

bool updateWalletBalance(const std::string& walletId, int newBalance) {
    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string sql = "UPDATE wallets SET balance=" + std::to_string(newBalance) +
                      " WHERE walletId='" + walletId + "';";
    bool ok = executeSQL(db, sql);
    sqlite3_close(db);
    return ok;
}