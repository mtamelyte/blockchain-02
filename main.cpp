#include "include/lib.h"

int main()
{
    int amountOfUsers = 2;
    int amountOfTransactions = 10;
    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
    createBlockchain(transactions, 2);
}