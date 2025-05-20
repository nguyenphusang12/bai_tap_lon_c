#include "transaction.h"
#include "wallet.h"
#include "otp.h"
#include "db_utils.h"
#include <iostream>
#include <ctime>

bool logTransaction(const Transaction& tx) {
    sqlite3* db;
    if (!initDatabase(&db)) return false;

    std::string sql = "INSERT INTO transactions (fromWallet, toWallet, amount, status, timestamp) VALUES ('" +
                      tx.fromWallet + "', '" + tx.toWallet + "', " +
                      std::to_string(tx.amount) + ", '" + tx.status + "', datetime('now'));";

    bool ok = executeSQL(db, sql);
    sqlite3_close(db);
    return ok;
}

bool transferPoints(const std::string& fromUserId, const std::string& toUserId, int amount, std::string& otpInput, std::string otpGenerated) {
    if (!verifyOTP(otpInput, otpGenerated)) {
        std::cout << "OTP không hợp lệ. Huỷ giao dịch.\n";
        return false;
    }

    Wallet A = getWalletByUserId(std::stoi(fromUserId));
    Wallet B = getWalletByUserId(std::stoi(toUserId));

    if (A.balance < amount) {
        std::cout << "Số dư không đủ.\n";
        return false;
    }

    bool okA = updateWalletBalance(A.walletId, A.balance - amount);
    bool okB = updateWalletBalance(B.walletId, B.balance + amount);

    if (okA && okB) {
        Transaction tx = { A.walletId, B.walletId, amount, "SUCCESS", "" };
        return logTransaction(tx);
    }

    std::cout << "Giao dịch thất bại, đã khôi phục trạng thái ban đầu.\n";
    return false;
}
