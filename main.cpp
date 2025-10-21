#include "include/lib.h"

int main()
{
    int amountOfUsers = 2;
    int amountOfTransactions = 1;
    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
}