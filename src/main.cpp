#include <iostream>
#include <string>
#include "db_utils.h"
#include "auth.h"
#include "admin_utils.h"
#include "backup_utils.h"
#include "wallet.h"
#include "transaction.h"
#include "otp.h"
#include "user.h"

void backupRestoreMenu() {
    int ch;
    std::cout << "\n--- SAO LƯU / PHỤC HỒI ---\n";
    std::cout << "1. Sao lưu cơ sở dữ liệu\n";
    std::cout << "2. Phục hồi dữ liệu từ bản sao lưu\n";
    std::cout << "0. Quay lại\nChọn: ";
    std::cin >> ch;

    switch (ch) {
        case 1: backupDatabase(); break;
        case 2: restoreDatabase(); break;
        case 0: break;
        default: std::cout << "Không hợp lệ.\n"; break;
    }
}

void userMenu(const User& user) {
    int choice;
    do {
        std::cout << "\n--- MENU NGƯỜI DÙNG ---\n";
        std::cout << "1. Xem số dư ví\n";
        std::cout << "2. Giao dịch chuyển điểm\n";
        std::cout << "3. Đổi mật khẩu\n";
        std::cout << "0. Đăng xuất\nChọn: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                Wallet w = getWalletByUserId(user.id);
                std::cout << "Số dư ví: " << w.balance << " điểm.\n";
                break;
            }
            case 2: {
                std::string toUsername;
                int amount;
                std::cout << "Nhập username người nhận: ";
                std::cin >> toUsername;

                int toUserId = getUserIdByUsername(toUsername);
                if (toUserId == -1) {
                    std::cout << "Không tìm thấy người dùng.\n";
                    break;
                }

                std::cout << "Số điểm: ";
                std::cin >> amount;
                std::string otp = generateOTP();
                std::cout << "[OTP gửi đến bạn: " << otp << "]\n";
                std::string input;
                std::cout << "Nhập OTP: ";
                std::cin >> input;
                transferPoints(std::to_string(user.id), std::to_string(toUserId), amount, input, otp);
                break;
            }
            case 3: {
                std::string newPass;
                std::cout << "Mật khẩu mới: ";
                std::cin >> newPass;
                changePassword(user.id, newPass);
                break;
            }
        }
    } while (choice != 0);
}

void adminMenu() {
    int choice;
    do {
        std::cout << "\n--- MENU QUẢN TRỊ ---\n";
        std::cout << "1. Xem danh sách người dùng\n";
        std::cout << "2. Tạo tài khoản mới\n";
        std::cout << "3. Sửa thông tin người dùng (OTP)\n";
        std::cout << "4. Sao lưu / Phục hồi\n";
        std::cout << "0. Đăng xuất\nChọn: ";
        std::cin >> choice;

        switch (choice) {
            case 1: listAllUsers(); break;
            case 2: createUserByAdmin(); break;
            case 3: updateUserByAdmin(); break;
            case 4: backupRestoreMenu(); break;
        }
    } while (choice != 0);
}

int main() {
    sqlite3* db;
    if (!initDatabase(&db)) return 1;
    sqlite3_close(db);

    int mainChoice;
    do {
        std::cout << "\n--- HỆ THỐNG QUẢN LÝ VÍ ĐIỂM THƯỞNG ---\n";
        std::cout << "1. Đăng nhập\n2. Đăng ký\n0. Thoát\nChọn: ";
        std::cin >> mainChoice;

        if (mainChoice == 1) {
            std::string username, password;
            std::cout << "Username: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;
            User user = loginUser(username, password);
            if (user.id == -1) {
                std::cout << "Đăng nhập thất bại.\n";
                continue;
            }
            if (user.requirePasswordChange) {
                std::string newPass;
                std::cout << "Đổi mật khẩu mới: ";
                std::cin >> newPass;
                changePassword(user.id, newPass);
            }

            if (user.role == ADMIN)
                adminMenu();
            else
                userMenu(user);
        }

        else if (mainChoice == 2) {
            User u;
            std::cout << "Username: ";
            std::cin >> u.username;
            std::cout << "Họ tên: ";
            std::cin.ignore(); std::getline(std::cin, u.fullName);
            std::cout << "Email: ";
            std::getline(std::cin, u.email);
            std::string password;
            std::cout << "Mật khẩu: ";
            std::cin >> password;

            u.hashedPassword = hashPassword(password);
            u.role = USER;
            u.requirePasswordChange = false;

            if (registerUser(u)) {
                std::cout << "Tạo tài khoản thành công.\n";
                createWalletForUser(u.id);
            } else {
                std::cout << "Tạo tài khoản thất bại.\n";
            }
        }
    } while (mainChoice != 0);

    std::cout << "Thoát chương trình.\n";
    return 0;
}