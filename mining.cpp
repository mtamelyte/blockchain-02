#include "include/lib.h"

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

std::vector<int> transactionsToBlock(std::vector<Transaction> transactions, int blockSize)
{
    std::vector<int> transactionsToBlock;
    std::vector<bool> isPicked;

    isPicked.assign(transactions.size(), false);

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> tx(0, transactions.size() - 1);

    for (int i = 0; i < blockSize; i++)
    {
        int it = tx(mt);

        if (!isPicked[it])
        {
            transactionsToBlock.push_back(it);
            isPicked[it] = true;
        }
    }

    return transactionsToBlock;
}

void removeTransactionsFromList(std::vector<int> transactionIterators, std::vector<Transaction> &transactions)
{
    for (auto tx : transactionIterators)
    {
        transactions.erase(transactions.begin() + tx);
    }
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty, std::vector<User> users)
{
    // initializes the blockchain
    std::list<Block> blockchain;

    std::stringstream buffer;

    while (!transactions.empty())
    {
        std::vector<int> transactionIterators = transactionsToBlock(transactions, blockSize);
        std::vector<Transaction> txToBlock;

        // adds transactions to a separate list to add to the block
        for (int i = 0; i < blockSize; i++)
        {
            // verify transaction id first
            std::string idPreHash = transactions[transactionIterators[i]].getSenderId() + transactions[transactionIterators[i]].getReceiverId() + std::to_string(transactions[transactionIterators[i]].getAmount());
            std::string txId = hash(idPreHash);

            if (txId != transactions[transactionIterators[i]].getTransactionId())
            {
                buffer << "Error: transaction ID doesn't match. Transaction #" << transactionIterators[i] << " skipped." << std::endl;
                std::cout << "Error: transaction ID doesn't match. Transaction #" << transactionIterators[i] << " skipped." << std::endl;
                break;
            }

            txToBlock.push_back(transactions[transactionIterators[i]]);
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
        removeTransactionsFromList(transactionIterators, transactions);
        txToBlock.clear();
    }
}