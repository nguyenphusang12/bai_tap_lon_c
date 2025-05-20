#ifndef AUTH_H
#define AUTH_H

#include "user.h"
#include <string>

std::string hashPassword(const std::string& password);
std::string generatePassword();
bool registerUser(User& user); 
User loginUser(const std::string& username, const std::string& password);
bool changePassword(int userId, const std::string& newPassword);
bool userExists(const std::string& username);

#endif