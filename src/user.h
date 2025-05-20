#ifndef USER_H
#define USER_H

#include <string>

enum Role { USER, ADMIN };

struct User {
    int id;
    std::string username;
    std::string hashedPassword;
    std::string fullName;
    std::string email;
    Role role;
    bool requirePasswordChange;
};
int getUserIdByUsername(const std::string& username);

#endif