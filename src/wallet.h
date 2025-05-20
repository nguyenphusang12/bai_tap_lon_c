#ifndef WALLET_H
#define WALLET_H

#include <string>

struct Wallet {
    std::string walletId;
    int userId;
    int balance;
};

bool createWalletForUser(int userId);
Wallet getWalletByUserId(int userId);
bool updateWalletBalance(const std::string& walletId, int newBalance);

#endif