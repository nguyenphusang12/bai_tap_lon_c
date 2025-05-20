#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

struct Transaction {
    std::string fromWallet;
    std::string toWallet;
    int amount;
    std::string status;
    std::string timestamp;
};

bool logTransaction(const Transaction& tx);
bool transferPoints(const std::string& fromUserId, const std::string& toUserId, int amount, std::string& otpInput, std::string otpGenerated);

#endif