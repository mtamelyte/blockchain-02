#include "include/lib.h"

int main()
{
    int amountOfUsers = 1000;
    const int amountOfTransactions = 10000;
    const int transactionsInBlock = 100;
    const int difficulty = 3;

    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
    createBlockchain(transactions, transactionsInBlock, difficulty, users);
}
// todo
// writing logs to file
// basic ui (pick amount of users, transactions, difficulty and block size)
// mining for a limited time
// readme
// parallel mining
// sorting utxos
// changing transaction utxo logic to go from largest to smallest
// improve transaction picking (pick at random instead of randomizing first and then going in a row)
// changing difficulty as we go on??
// allowing the generated users to mine and get rewards??
// refactor where possible