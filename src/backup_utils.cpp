#include "backup_utils.h"
#include <filesystem>
#include <iostream>

bool backupDatabase() {
    try {
        std::filesystem::copy_file(
            "db/reward_wallet.db",
            "backup/reward_wallet_backup.db",
            std::filesystem::copy_options::overwrite_existing
        );
        std::cout << "Sao lưu thành công vào: backup/reward_wallet_backup.db\n";
        return true;
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Sao lưu thất bại: " << e.what() << "\n";
        return false;
    }
}

bool restoreDatabase() {
    try {
        std::filesystem::copy_file(
            "backup/reward_wallet_backup.db",
            "db/reward_wallet.db",
            std::filesystem::copy_options::overwrite_existing
        );
        std::cout << "Phục hồi thành công từ bản sao lưu.\n";
        return true;
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Phục hồi thất bại: " << e.what() << "\n";
        return false;
    }
}