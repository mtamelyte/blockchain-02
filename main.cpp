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

    std::vector<User> users = generateUsers(amountOfUsers);
    std::vector<Transaction> transactions = generateTransactions(amountOfTransactions, users);
    createBlockchain(transactions, transactionsInBlock, difficulty, users);
}
// todo
// mining for a limited time
// readme
// parallel mining
// sorting utxos
// changing transaction utxo logic to go from largest to smallest
// improve transaction picking (pick at random instead of randomizing first and then going in a row)
// changing difficulty as we go on??
// allowing the generated users to mine and get rewards??
// refactor where possible(log output)