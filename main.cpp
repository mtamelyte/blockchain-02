#include "include/lib.h"

int main()
{
    int amountOfUsers = 1000;
    int amountOfTransactions = 10000;
    int transactionsInBlock = 100;
    int difficulty = 3;

    std::cout << "How many users would you like to generate?" << std::endl;
    std::cin >> amountOfUsers;
    std::cout << "How many transactions would you like to generate?" << std::endl;
    std::cin >> amountOfTransactions;
    std::cout << "What should the block size be?" << std::endl;
    std::cin >> transactionsInBlock;
    std::cout << "What should the mining difficulty be?" << std::endl;
    std::cin >> difficulty;
    std::cout << std::endl;

    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
    createBlockchain(transactions, transactionsInBlock, difficulty, users);
}