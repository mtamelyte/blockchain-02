#include "include/lib.h"

UTXO createUtxo(std::string userName, std::string userKey, int amount)
{
    std::string idToHash = userName + userKey + std::to_string(amount) + generateSalt();
    std::string id = hash(idToHash);

    UTXO utxo;
    utxo.id = id;
    utxo.amount = amount;

    return utxo;
}

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

std::vector<User> generateUsers(int userAmount)
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
            int amount = amt(mt);
            UTXO utxo = createUtxo(user.getName(), user.getPublicKey(), amount);
        }

        users.push_back(user);
    }

    return users;
}

std::vector<Transaction> generateTransactions(const int txAmount, std::vector<User> &users)
{
    std::vector<Transaction> transactions;

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> num(0, 999);
    std::uniform_int_distribution<> amt(1, 5000);

    for (int i = 0; i < txAmount; i++)
    {
        // randomly pick a sender and a receiver
        int senderIt = num(mt);
        int receiverIt = num(mt);

        // ensure the sender and the receiver aren't the same person and the sender has money they can transfer
        while (senderIt == receiverIt || users[senderIt].getBalance() == 0)
        {
            // if they are, regenerate the sender
            senderIt = num(mt);
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
        std::vector<UTXO> input;
        std::vector<UTXO> output;

        for (auto utxo : utxos)
        {
            // if the current utxo is not enough to close the transaction, move it to the receiver, deduct the amount and keep going
            if (utxo.amount < remainingAmount)
            {
                output.push_back(utxo);
                UTXO newUtxo = createUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount);
                input.push_back(newUtxo);
                remainingAmount -= utxo.amount;
            }

            // if the current utxo is exactly enough, move it to the receiver and break the loop
            else if (utxo.amount == remainingAmount)
            {
                output.push_back(utxo);
                UTXO newUtxo = createUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount);
                input.push_back(newUtxo);
                remainingAmount = 0;
                break;
            }

            // if the current utxo is bigger than the remaining amount, remove it, split it into the needed sum and the remainder and add them back to respective users
            else
            {
                output.push_back(utxo);
                UTXO newUtxo = createUtxo(receiver.getName(), receiver.getPublicKey(), remainingAmount);
                UTXO change = createUtxo(sender.getName(), sender.getPublicKey(), utxo.amount - remainingAmount);
                input.push_back(newUtxo);
                input.push_back(change);
                remainingAmount = 0;
                break;
            }
        }

        // create transaction id - a hash of all other data
        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);

        // construct a transaction with all the generated data and add it to the list
        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer, input, output);
        transactions.push_back(tx);

        // ensure the changes are updated in the list of users as well
        users[senderIt] = sender;
        users[receiverIt] = receiver;

        std::cout << i << std::endl;
        std::cout << "Siuntėjas: " << sender.getName() << ", gavėjas: " << receiver.getName() << ". Pervesta " << amountToTransfer << std::endl;
        std::cout << sender.getName() << ": " << sender.getBalance() << " " << receiver.getName() << ": " << receiver.getBalance() << std::endl;
        std::cout << std::endl;
    }

    return transactions;
}

Block mineBlock(std::string previousBlockHash, std::string merkleRootHash, int difficulty)
{
    // initializes proof of work
    int nonce = 0;

    // creates the difficulty (how many zeroes in a row is required)
    std::string diff = "";
    diff.append(difficulty, '0');

    while (true)
    {
        // creates a new block with the current nonce and hashes it
        Block newBlock(previousBlockHash, merkleRootHash, nonce++, difficulty);
        std::string hash = newBlock.calculateBlockHash();

        // checks if it matches the difficulty
        if (hash.substr(0, 3) == diff)
        {
            // if so, the mining function ends
            return newBlock;
            break;
        }
    }
}

void moveUTXOs(std::vector<UTXO> input, std::vector<UTXO> output)
{
    for (UTXO utxo : input)
    {
    }
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty)
{
    // randomizes the transaction list
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::shuffle(transactions.begin(), transactions.end(), mt);

    // initializes the variables
    std::vector<Transaction> txToBlock;
    std::list<Block> blockchain;

    while (!transactions.empty())
    {
        std::string allTransactionsToHash = "";

        // separate txSize variable because the size of the transactions vector changes every loop
        int txSize = transactions.size();

        // adds transactions to a separate list to add to the block and removes them from the main list
        for (int i = txSize - 1; i >= (txSize - (blockSize)) && !transactions.empty(); i--)
        {
            txToBlock.push_back(transactions[i]);
            allTransactionsToHash += transactions[i].getTransactionId();
            transactions.pop_back();
        }

        // gets the previous block hash
        std::string previousBlockHash;
        if (blockchain.empty())
            previousBlockHash.append(64, '0');
        else
            previousBlockHash = blockchain.back().getBlockHash();

        // mines the block, adds the transaction record and connects it to the blockchain
        Block newBlock = mineBlock(previousBlockHash, hash(allTransactionsToHash), difficulty);
        newBlock.setTransactions(txToBlock);
        blockchain.push_back(newBlock);
    }
}