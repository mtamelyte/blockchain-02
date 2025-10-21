#include "include/lib.h"

std::string generateSalt()
{
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> utfCode(33, 126);

    std::string salt = "";
    for (int i = 0; i < 10; i++)
    {
        salt += (char)utfCode(mt);
    }
    return salt;
}

std::vector<User> generateUsers(double userAmount)
{
    std::vector<User> users;
    for (int i = 1; i <= 1000; i++)
    {
        std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> amt(100, 1000);

        const std::string name = "User_" + std::to_string(i);

        const std::string preHashPublicKey = "User_" + std::to_string(i) + generateSalt();
        const std::string publicKey = hash(preHashPublicKey);

        User user(name, publicKey);
        for (int j = 0; j < 10; j++)
        {
            double amount = amt(mt);
            std::cout << amount << " " << std::endl;
            user.addUTXO(amount);
        }
        std::cout << user.getName() << " " << user.getPublicKey() << " " << user.getBalance() << std::endl;
        users.push_back(user);
    }

    return users;
}