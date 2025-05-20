#ifndef OTP_H
#define OTP_H

#include <string>

std::string generateOTP();
bool verifyOTP(const std::string& input, const std::string& actual);

#endif