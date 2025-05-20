#include "admin_utils.h"
#include "auth.h"
#include "otp.h"
#include "db_utils.h"
#include <iostream>
#include <sqlite3.h>

void listAllUsers() {
    sqlite3* db;
    if (!initDatabase(&db)) return;

    std::string sql = "SELECT id, username, fullName, email, role FROM users;";
    auto callback = [](void*, int, char** argv, char**) -> int {
        std::cout << "ID: " << argv[0]
                  << " | Username: " << argv[1]
                  << " | Full Name: " << argv[2]
                  << " | Email: " << argv[3]
                  << " | Role: " << (std::string(argv[4]) == "1" ? "ADMIN" : "USER") << "\n";
        return 0;
    };

    userQuery(db, sql, callback, nullptr);
    sqlite3_close(db);
}

bool createUserByAdmin() {
    User u;
    std::cout << "Nhập username: "; std::cin >> u.username;
    std::cout << "Họ tên: "; std::cin.ignore(); std::getline(std::cin, u.fullName);
    std::cout << "Email: "; std::getline(std::cin, u.email);

    std::string passChoice;
    std::cout << "Bạn có muốn tự nhập mật khẩu? (y/n): ";
    std::cin >> passChoice;

    std::string pwd;
    if (passChoice == "y" || passChoice == "Y") {
        std::cout << "Nhập mật khẩu: ";
        std::cin >> pwd;
        u.requirePasswordChange = false;
    } else {
        pwd = generatePassword();
        std::cout << "Mật khẩu được sinh tự động: " << pwd << "\n";
        u.requirePasswordChange = true;
    }

    u.hashedPassword = hashPassword(pwd);
    u.role = USER;

    return registerUser(u);
}

bool updateUserByAdmin() {
    std::string username;
    std::cout << "Nhập username người cần chỉnh sửa: ";
    std::cin >> username;

    std::string otp = generateOTP();
    std::cout << "[OTP đã gửi đến người dùng: " << otp << "]\n";

    std::string otpInput;
    std::cout << "Nhập OTP xác nhận: ";
    std::cin >> otpInput;

    if (!verifyOTP(otpInput, otp)) {
        std::cout << "OTP không hợp lệ. Huỷ thao tác.\n";
        return false;
    }

    std::string fullName, email;
    std::cout << "Nhập tên mới: ";
    std::cin.ignore();
    std::getline(std::cin, fullName);
    std::cout << "Nhập email mới: ";
    std::getline(std::cin, email);

    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string sql = "UPDATE users SET fullName='" + fullName + "', email='" + email +
                      "' WHERE username='" + username + "';";

    return executeSQL(db, sql);
}