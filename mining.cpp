#include "include/lib.h"

std::vector<Transaction> transactionsToBlock(std::vector<Transaction> &transactions, int blockSize, std::stringstream &buffer)
{
    std::vector<Transaction> transactionsToBlock;
    std::vector<bool> isPicked;

    int listSize = std::min(blockSize, static_cast<int>(transactions.size()));

    isPicked.assign(transactions.size(), false);

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> tx(0, transactions.size() - 1);

    for (int i = 0; i < listSize; i++)
    {
        int it = tx(mt);

        if (!isPicked[it])
        {
            // verify transaction id first
            std::string idPreHash = transactions[it].getSenderId() + transactions[it].getReceiverId() + std::to_string(transactions[it].getAmount());
            std::string txId = hash(idPreHash);

            if (txId != transactions[it].getTransactionId())
            {
                buffer << "Error: transaction ID doesn't match. Transaction #" << tx << " skipped." << std::endl;
                continue;
            }

            transactionsToBlock.push_back(transactions[it]);
            isPicked[it] = true;
        }
        else
            i--;
    }

    return transactionsToBlock;
}

// from https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
int findUser(std::vector<User> &users, std::string userId)
{
    auto it = find_if(users.begin(), users.end(), [&userId](const User &obj)
                      { return obj.getPublicKey() == userId; });

    if (it != users.end())
    {
        auto index = std::distance(users.begin(), it);

        return index;
    }
}

int findTransaction(std::vector<Transaction> &transactions, std::string transactionId)
{
    auto it = find_if(transactions.begin(), transactions.end(), [&transactionId](const Transaction &obj)
                      { return obj.getTransactionId() == transactionId; });

    if (it != transactions.end())
    {
        auto index = std::distance(transactions.begin(), it);

        return index;
    }
}

void moveUTXOs(std::vector<Transaction> &txToBlock, std::vector<User> &users)
{
    // go through all the transactions that were included in the block
    for (auto tx : txToBlock)
    {
        // get the sender
        User &sender = users[findUser(users, tx.getSenderId())];

        std::vector<UTXO> utxos = sender.getUTXOs();
        for (auto utxo : utxos)
        {
            if (utxo.txId == tx.getTransactionId() && utxo.used)
            {
                sender.removeUTXO(utxo.id);
            }
        }

        // get the receiver
        User &receiver = users[findUser(users, tx.getReceiverId())];

        // go through all the outputs of a transaction and move them to the appropriate receivers
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

void removeTransactionsFromList(std::vector<Transaction> &transactionsInBlock, std::vector<Transaction> &transactions)
{
    for (auto tx : transactionsInBlock)
    {
        transactions.erase(transactions.begin() + findTransaction(transactions, tx.getTransactionId()));
    }
}

Block mineBlock(std::string previousBlockHash, int difficulty, std::stringstream &buffer, std::vector<Transaction> &transactions, int blockSize, std::vector<User> &users)
{
    std::atomic<bool> isFound = false;
    Block minedBlock;
    std::vector<std::vector<Transaction>> candidateBlocks;

    int maxAttempts = 1000;

    std::string diff = "";
    diff.append(difficulty, '0');

#pragma omp parallel num_threads(5) default(none) shared(candidateBlocks, maxAttempts, previousBlockHash, difficulty, diff, minedBlock, buffer, std::cout, transactions, blockSize, isFound, users)
    {
        std::vector<Transaction> txToBlock = transactionsToBlock(transactions, blockSize);
#pragma omp critical
        {
            candidateBlocks.push_back(txToBlock);
        }
    }

multithreadMining:

#pragma omp parallel num_threads(5) default(none) shared(candidateBlocks, maxAttempts, previousBlockHash, difficulty, diff, minedBlock, buffer, std::cout, transactions, blockSize, isFound, users)
{

    int threadId = omp_get_thread_num();
    int numOfThreads = omp_get_num_threads();

    std::vector<Transaction> txToBlock = candidateBlocks[threadId];

    int nonce = threadId;
    int attemptCount = 0;

    while (!isFound)
    {
        attemptCount++;
        if (attemptCount > maxAttempts)
            break;

        Block newBlock(previousBlockHash, MerkleTree(txToBlock).getRoot(), nonce += 5, difficulty);
        std::string hash = newBlock.calculateBlockHash();

        if (hash.substr(0, difficulty) == diff)
        {
            bool expected = false;
            if (isFound.compare_exchange_strong(expected, true))
            {
#pragma omp critical
                {
                    std::cout << "Thread " << threadId << " won the race! (Total " << numOfThreads << " threads available)" << std::endl;
                    minedBlock = newBlock;
                    moveUTXOs(txToBlock, users);
                    removeTransactionsFromList(txToBlock, transactions);
                }
            }
            break;
        }
    }
}

    if (!isFound)
    {
        std::cout << "Failed to mine a block in " << maxAttempts << " attempts. ";
        maxAttempts *= 2;
        std::cout << "Retrying with " << maxAttempts << " attempts." << std::endl;
        goto multithreadMining;
    }
    return minedBlock;
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty, std::vector<User> &users)
{
    std::list<Block> blockchain;

    std::stringstream buffer;

    while (!transactions.empty())
    {
        std::string previousBlockHash;
        Block newBlock;

        if (blockchain.empty())
            previousBlockHash.append(64, '0');
        else
            previousBlockHash = blockchain.back().getBlockHash();

        std::cout << "Block #" << blockchain.size() << std::endl;
        newBlock = mineBlock(previousBlockHash, difficulty, buffer, transactions, blockSize, users);
        blockchain.push_back(newBlock);

        std::cout << newBlock << std::endl;
        buffer << newBlock << std::endl;

        std::ofstream fout;
        fout.open("mining-log.txt");
        fout << buffer.str();
        fout.close();
    }
}