#include "auth.h"
#include "db_utils.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <sqlite3.h>
#include <iostream>

std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)password.c_str(), password.size(), hash);
    std::ostringstream oss;
    for (unsigned char i : hash)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
    return oss.str();
}

std::string generatePassword() {
    const char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(chars) - 2);
    std::string pwd;
    for (int i = 0; i < 10; ++i)
        pwd += chars[dist(rng)];
    return pwd;
}

bool userExists(const std::string& username) {
    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string sql = "SELECT COUNT(*) FROM users WHERE username='" + username + "';";
    int count = 0;

    auto callback = [](void* data, int, char** argv, char**) -> int {
        int* count = static_cast<int*>(data);
        *count = std::stoi(argv[0]);
        return 0;
    };

    userQuery(db, sql, callback, &count);
    sqlite3_close(db);
    return count > 0;
}

bool registerUser(User& user) {
    if (userExists(user.username)) {
        std::cout << "Username already exists.\n";
        return false;
    }

    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string sql = "INSERT INTO users (username, password, fullName, email, role, requirePasswordChange) VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Lỗi prepare: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.hashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user.fullName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user.email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, user.role);
    sqlite3_bind_int(stmt, 6, user.requirePasswordChange ? 1 : 0);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (success) {
        user.id = sqlite3_last_insert_rowid(db); 
    } else {
        std::cerr << "Lỗi khi thêm user: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

User loginUser(const std::string& username, const std::string& password) {
    sqlite3* db;
    User user = { -1, "", "", "", "", USER, false };

    if (!initDatabase(&db)) return user;

    std::string hashed = hashPassword(password);
    std::string sql = "SELECT id, username, password, fullName, email, role, requirePasswordChange FROM users WHERE username='" + username + "';";

    auto callback = [](void* data, int, char** argv, char**) -> int {
        User* user = static_cast<User*>(data);
        user->id = std::stoi(argv[0]);
        user->username = argv[1];
        user->hashedPassword = argv[2];
        user->fullName = argv[3];
        user->email = argv[4];
        user->role = static_cast<Role>(std::stoi(argv[5]));
        user->requirePasswordChange = std::stoi(argv[6]);
        return 0;
    };

    userQuery(db, sql, callback, &user);
    sqlite3_close(db);

    if (user.id != -1 && user.hashedPassword == hashed)
        return user;

    user.id = -1;
    return user;
}

bool changePassword(int userId, const std::string& newPassword) {
    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string hash = hashPassword(newPassword);
    std::string sql = "UPDATE users SET password='" + hash + "', requirePasswordChange=0 WHERE id=" + std::to_string(userId) + ";";

    bool ok = executeSQL(db, sql);
    sqlite3_close(db);
    return ok;
}