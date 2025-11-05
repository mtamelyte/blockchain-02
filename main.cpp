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
// readme
// improve logs
// sorting utxos
// changing transaction utxo logic to go from largest to smallest
// implement reading from logs
// figure something out with the running out of utxos issue (maybe add a constraint that allows the transaction count to be up to 50 times larger than the user count?)
// fix the goto in parallel mining
// changing difficulty as we go on??
// allowing the generated users to mine and get rewards??
// refactor where possible(log output)