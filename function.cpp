#include "include/lib.h"
#include <ranges>

std::string generateSalt()
{
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> utfCode(33, 126);

    std::string salt = "";

    for (int i = 0; i < 10; i++)
    {
        salt += (char)utfCode(mt); // generate a random UTF-8 code and change it to a corresponding symbol
    }

    return salt;
}

std::vector<User> generateUsers(double userAmount)
{
    std::vector<User> users;
    for (int i = 1; i <= userAmount; i++)
    {
        std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> amt(100, 1000);

        const std::string name = "User_" + std::to_string(i); // create a username like "User_45"

        // create a public key
        const std::string preHashPublicKey = "User_" + std::to_string(i) + generateSalt();
        const std::string publicKey = hash(preHashPublicKey);

        User user(name, publicKey); // construct a user with the specified username and public key

        // generate 10 UTXOs for each user
        for (int j = 0; j < 10; j++)
        {
            double amount = amt(mt);
            user.addUTXO(amount);
        }

        users.push_back(user);
    }

    return users;
}

std::vector<Transaction> generateTransactions(int txAmount, std::vector<User> &users)
{
    std::vector<Transaction> transactions;

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> num(0, 1);
    std::uniform_int_distribution<> amt(1, 5000);

    for (int i = 0; i < txAmount; i++)
    {
        // randomly pick a sender and a receiver
        int senderIt = num(mt);
        int receiverIt = num(mt);

        // ensure the sender and the receiver aren't the same person
        while (senderIt == receiverIt) // ensure the sender and the receiver aren't the same person
        {
            // if they are, regenerate the receiver
            receiverIt = num(mt);
        }

        // get the sender and receiver as objects
        User sender = users[senderIt];
        User receiver = users[receiverIt];

        // randomly generate an amount to transfer
        double amountToTransfer = amt(mt);

        // ensure the sender has enough for the transfer
        while (amountToTransfer > sender.getBalance())
        {
            // if they don't, pick a new amount to transfer
            amountToTransfer = amt(mt);
        }
        // get the existing UTXOs
        std::vector<UTXO> utxos = sender.getUTXOs();
        double remainingAmount = amountToTransfer;

        for (auto utxo : utxos)
        {
            // if the current utxo is not enough to close the transaction, move it to the receiver, deduct the amount and keep going
            if (utxo.amount < remainingAmount)
            {
                sender.removeUTXO(utxo.id);
                receiver.addUTXO(utxo.amount);
                remainingAmount -= utxo.amount;
            }
            // if the current utxo is exactly enough, move it to the receiver and break the loop
            else if (utxo.amount == remainingAmount)
            {
                sender.removeUTXO(utxo.id);
                receiver.addUTXO(utxo.amount);
                remainingAmount = 0;
                break;
            }
            // if the current utxo is bigger than the remaining amount, remove it, split it into the needed sum and the remainder and add them back to respective users
            else
            {
                sender.removeUTXO(utxo.id);
                receiver.addUTXO(remainingAmount);
                sender.addUTXO(utxo.amount - remainingAmount);
                remainingAmount = 0;
                break;
            }
        }

        // create transaction id - a hash of all other data
        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);
        std::cout << txId << std::endl;

        // construct a transaction with all the generated data and add it to the list
        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer);
        transactions.push_back(tx);

        // ensure the changes are updated in the list of users as well
        users[senderIt] = sender;
        users[receiverIt] = receiver;

        std::cout << "Siuntėjas: " << sender.getName() << ", gavėjas: " << receiver.getName() << ". Pervesta " << amountToTransfer << std::endl;
    }

    return transactions;
}

Block mineBlock(std::string previousBlockHash, std::string merkleRootHash)
{
    int nonce = 0;
    std::string difficulty = "000";
    while (true)
    {
        Block newBlock(previousBlockHash, merkleRootHash, nonce++, 3);
        std::string hash = newBlock.calculateBlockHash();
        std::cout << hash << std::endl;
        if (hash.substr(0, 3) == difficulty)
        {
            return newBlock;
            break;
        }
    }
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize)
{
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::shuffle(transactions.begin(), transactions.end(), mt);
    std::vector<Transaction> txToBlock;
    std::string allTransactionsToHash = "";
    std::list<Block> blockchain;

    while (!transactions.empty())
    {
        int txSize = transactions.size();
        for (int i = txSize - 1; i >= (txSize - (blockSize)) && !transactions.empty(); i--)
        {
            txToBlock.push_back(transactions[i]);
            allTransactionsToHash += transactions[i].getTransactionId();
            transactions.pop_back();
        }
        std::string previousBlockHash;
        if (blockchain.empty())
            previousBlockHash.append(64, '0');
        else
            previousBlockHash = blockchain.back().getBlockHash();
        Block newBlock = mineBlock(previousBlockHash, hash(allTransactionsToHash));
        newBlock.setTransactions(txToBlock);
        blockchain.push_back(newBlock);
    }
}