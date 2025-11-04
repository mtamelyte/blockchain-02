#include "include/lib.h"

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
        std::uniform_int_distribution<> amt(50, 500);

        const std::string name = "User_" + std::to_string(i); // create a username like "User_45"

        // create a public key
        const std::string preHashPublicKey = "User_" + std::to_string(i) + generateSalt();
        const std::string publicKey = hash(preHashPublicKey);

        User user(name, publicKey); // construct a user with the specified username and public key

        // generate 50 UTXOs for each user
        for (int j = 0; j < 50; j++)
        {
            int amount = amt(mt);
            UTXO utxo(user.getName(), user.getPublicKey(), amount, "", 0);
            user.addUTXO(utxo);
        }

        users.push_back(user);
    }

    return users;
}

std::vector<Transaction> generateTransactions(const int txAmount, std::vector<User> &users)
{
    std::vector<Transaction> transactions;

    // prepare for writing to file
    std::stringstream buffer;

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> num(0, users.size() - 1);
    std::uniform_int_distribution<> amt(1, 1000);

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

        // create transaction id - a hash of all other data
        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);

        std::vector<UTXO> output;

        std::cout << "Transaction #" << i + 1 << std::endl;
        std::cout << "Sender: " << sender.getName() << ", receiver: " << receiver.getName() << ". Amount transferred: " << amountToTransfer << std::endl;
        std::cout << "Inputs: " << std::endl;

        buffer << "Transaction #" << i + 1 << std::endl;
        buffer << "Sender: " << sender.getName() << ", receiver: " << receiver.getName() << ". Amount transferred: " << amountToTransfer << std::endl;
        buffer << "Inputs: " << std::endl;

        for (auto &utxo : utxos)
        {
            // if the current utxo is not enough to close the transaction, move it to the receiver, deduct the amount and keep going
            if (utxo.used)
            {
                continue;
            }
            else if (utxo.amount < remainingAmount)
            {
                utxo.used = true;
                utxo.txId = txId;

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount, txId, 0);
                output.push_back(newUtxo);

                remainingAmount -= utxo.amount;
            }

            // if the current utxo is exactly enough, move it to the receiver and break the loop
            else if (utxo.amount == remainingAmount)
            {
                utxo.used = true;
                utxo.txId = txId;

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount, txId, 0);
                output.push_back(newUtxo);

                remainingAmount = 0;
                break;
            }

            // if the current utxo is bigger than the remaining amount, remove it, split it into the needed sum and the remainder and add them back to respective users
            else
            {
                utxo.used = true;
                utxo.txId = txId;

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), remainingAmount, txId, 0);
                UTXO change(sender.getName(), sender.getPublicKey(), utxo.amount - remainingAmount, txId, 1);

                output.push_back(newUtxo);
                output.push_back(change);

                remainingAmount = 0;
                break;
            }
        }

        std::cout << "Outputs: " << std::endl;
        buffer << "Outputs:" << std::endl;
        for (auto utxo : output)
        {
            std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount;
            buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount;
            if (utxo.changeFlag)
            {
                std::cout << " (change)";
                buffer << " (change)";
            }
            std::cout << std::endl;
            buffer << std::endl;
        }

        // update the utxo status in the sender's data
        sender.setUTXOs(utxos);

        // construct a transaction with all the generated data and add it to the list
        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer, output);
        transactions.push_back(tx);

        // ensure the changes are updated in the list of users as well
        users[senderIt] = sender;
        users[receiverIt] = receiver;

        std::cout << std::endl;
        buffer << std::endl;
    }

    std::ofstream fout;
    fout.open("transaction-log.txt");
    fout << buffer.str();
    fout.close();

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
        if (hash.substr(0, difficulty) == diff)
        {
            // if so, the mining function ends
            return newBlock;
            break;
        }
    }
}

// from https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
int findUser(std::vector<User> users, std::string userId)
{
    auto it = find_if(users.begin(), users.end(), [&userId](const User &obj)
                      { return obj.getPublicKey() == userId; });

    if (it != users.end())
    {
        auto index = std::distance(users.begin(), it);

        return index;
    }
}

void moveUTXOs(std::vector<Transaction> &txToBlock, std::vector<User> users)
{
    // go through all the transactions that were included in the block
    for (auto tx : txToBlock)
    {
        // get the sender
        User sender = users[findUser(users, tx.getSenderId())];

        // go through all the sender utxos and remove the used ones
        std::vector<UTXO> utxos = sender.getUTXOs();
        for (auto utxo : utxos)
        {
            if (utxo.txId == tx.getTransactionId() && utxo.used == true)
            {
                sender.removeUTXO(utxo.id);
            }
        }

        // get the receiver
        User receiver = users[findUser(users, tx.getReceiverId())];

        // go through all the outputs of a transaction and move them to the approporiate receivers
        std::vector<UTXO> outputs = tx.getOutputs();
        for (auto utxo : outputs)
        {
            if (utxo.changeFlag)
            {
                sender.addUTXO(utxo);
            }
            else
                receiver.addUTXO(utxo);
        }
    }
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty, std::vector<User> users)
{
    // randomizes the transaction list
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::shuffle(transactions.begin(), transactions.end(), mt);

    // initializes the variables
    std::vector<Transaction> txToBlock;
    std::list<Block> blockchain;

    std::stringstream buffer;

    while (!transactions.empty())
    {
        std::string allTransactionsToHash = "";

        // separate txSize variable because the size of the transactions vector changes every loop
        int txSize = transactions.size();

        // adds transactions to a separate list to add to the block and removes them from the main list
        for (int i = txSize - 1; i >= (txSize - (blockSize)) && !transactions.empty(); i--)
        {
            // verify transaction id first
            std::string idPreHash = transactions[i].getSenderId() + transactions[i].getReceiverId() + std::to_string(transactions[i].getAmount());
            std::string txId = hash(idPreHash);

            if (txId != transactions[i].getTransactionId())
            {
                buffer << "Error: transaction ID doesn't match. Transaction skipped." << std::endl;
                std::cout << "Error: transaction ID doesn't match. Transaction skipped." << std::endl;
                transactions.pop_back();
                break;
            }

            txToBlock.push_back(transactions[i]);
            transactions.pop_back();
        }

        // gets the previous block hash
        std::string previousBlockHash;
        if (blockchain.empty())
            previousBlockHash.append(64, '0');
        else
            previousBlockHash = blockchain.back().getBlockHash();

        // mines the block, adds the transaction record and connects it to the blockchain
        Block newBlock = mineBlock(previousBlockHash, merkleRootHash(txToBlock), difficulty);
        newBlock.setTransactions(txToBlock);
        blockchain.push_back(newBlock);

        std::cout << "New block mined! " << std::endl;
        std::cout << "Difficulty: " << difficulty << std::endl;
        std::cout << "Hash: " << newBlock.getBlockHash() << std::endl;
        std::cout << "Nonce: " << newBlock.getNonce() << std::endl;
        std::cout << std::endl;

        buffer << "New block mined! " << std::endl;
        buffer << "Difficulty: " << difficulty << std::endl;
        buffer << "Hash: " << newBlock.getBlockHash() << std::endl;
        buffer << "Nonce: " << newBlock.getNonce() << std::endl;
        buffer << std::endl;

        // write log to file
        std::ofstream fout;
        fout.open("mining-log.txt");
        fout << buffer.str();
        fout.close();

        // officially moves all the utxos used and created in the transactions in the block
        moveUTXOs(txToBlock, users);
        txToBlock.clear();
    }
}