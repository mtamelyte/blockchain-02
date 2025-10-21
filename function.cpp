#include "include/lib.h"

std::vector<User> generateUsers(double userAmount)
{
    std::vector<User> users;
    for (int i = 1; i <= 1000; i++)
    {
        std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> amt(100, 1000000);
        std::string name = "User_" + std::to_string(i);
        std::string preHashPublicKey = "User_" + std::to_string(i) + std::to_string(mt());
        const std::string publicKey = hash(preHashPublicKey);
        const double amount = amt(mt);
        User user(name, publicKey, amount);
        users.push_back(user);
    }

    return users;
}