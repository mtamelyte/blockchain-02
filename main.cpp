#include "include/lib.h"

int main()
{
    const int amountOfUsers = 1000;
    const int amountOfTransactions = 10000;
    const int transactionsInBlock = 100;
    const int difficulty = 3;
    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
    createBlockchain(transactions, transactionsInBlock, difficulty);
}