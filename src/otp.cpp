#include "otp.h"
#include <random>
#include <ctime>

std::string generateOTP() {
    std::default_random_engine rng(std::time(nullptr));
    std::uniform_int_distribution<int> dist(100000, 999999);
    return std::to_string(dist(rng));
}

bool verifyOTP(const std::string& input, const std::string& actual) {
    return input == actual;
}