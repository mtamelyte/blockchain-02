#include "../include/lib.h"

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

std::vector<Transaction> transactionsToBlock(std::vector<Transaction> &transactions, int blockSize, std::stringstream &buffer, std::vector<User> &users)
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
            std::string idPreHash = transactions[it].getSenderId() + transactions[it].getReceiverId() + std::to_string(transactions[it].getAmount());
            std::string txId = hash(idPreHash);

            if (txId != transactions[it].getTransactionId())
            {
                buffer << "Error: transaction ID doesn't match. Transaction #" << tx << " skipped." << std::endl;
                User &sender = users[findUser(users, transactions[it].getSenderId())];
                std::unordered_map<std::string, UTXO> utxos = sender.getUTXOs();
                for (auto input : transactions[it].getInputs())
                {
                    UTXO &utxo = utxos.find(input)->second;
                    utxo.used = false;
                }
                sender.setUTXOs(utxos);
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

void moveUTXOs(std::vector<Transaction> &txToBlock, std::vector<User> &users)
{
    for (auto tx : txToBlock)
    {
        User &sender = users[findUser(users, tx.getSenderId())];
        User &receiver = users[findUser(users, tx.getReceiverId())];

        std::vector<std::string> inputs = tx.getInputs();
        for (auto utxo : inputs)
        {
            sender.removeUTXO(utxo);
        }

        std::unordered_map<std::string, UTXO> outputs = tx.getOutputs();
        for (auto utxo : outputs)
        {
            if (utxo.second.changeFlag)
            {
                sender.addUTXO(utxo.first, utxo.second);
            }
            else
                receiver.addUTXO(utxo.first, utxo.second);
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

bc::hash_digest txToHash(std::string tx)
{
    bc::hash_digest hash;
    bc::decode_hash(hash, tx);
    return hash;
}

bc::hash_list txHashes(std::vector<Transaction> &transactions)
{
    bc::hash_list hashes;

    for (auto tx : transactions)
    {
        hashes.push_back(txToHash(tx.getTransactionId()));
    }

    return hashes;
}

Block mineBlock(std::vector<std::vector<Transaction>> candidateBlocks, int maxAttempts, std::string &previousBlockHash, int difficulty, std::vector<User> &users, std::vector<Transaction> &transactions, std::stringstream &buffer)
{
    std::atomic<bool> isFound{false};
    Block minedBlock;

    std::string diff = "";
    diff.append(difficulty, '0');

#pragma omp parallel num_threads(5) default(none) shared(candidateBlocks, maxAttempts, previousBlockHash, difficulty, diff, minedBlock, buffer, std::cout, transactions, isFound, users)
    {

        int threadId = omp_get_thread_num();
        int numOfThreads = omp_get_num_threads();

        std::vector<Transaction> txToBlock = candidateBlocks[threadId];

        int nonce = threadId;
        int attemptCount = 0;

        bc::hash_list hashList = txHashes(txToBlock);

        while (!isFound)
        {
            attemptCount++;
            if (attemptCount > maxAttempts)
                break;

            Block newBlock(previousBlockHash, create_merkle(hashList), nonce += 5, difficulty);
            std::string hash = newBlock.calculateBlockHash();

            if (hash.substr(0, difficulty) == diff)
            {
                bool expected = false;
                if (isFound.compare_exchange_strong(expected, true))
                {
#pragma omp critical
                    {
                        std::cout << "Thread " << threadId << " mined a new block!" << std::endl;
                        minedBlock = newBlock;
                        minedBlock.setTransactions(txToBlock);
                        moveUTXOs(txToBlock, users);
                        removeTransactionsFromList(txToBlock, transactions);
                    }
                }
                break;
            }
        }
    }

    if (isFound)
        return minedBlock;
    else
    {
        std::cout << "Failed to mine a block in " << maxAttempts << " attempts. ";
        maxAttempts *= 2;
        std::cout << "Retrying with " << maxAttempts << " attempts." << std::endl;
        return mineBlock(candidateBlocks, maxAttempts, previousBlockHash, difficulty, users, transactions, buffer);
    }
}

Block parallelMining(std::string previousBlockHash, int difficulty, std::stringstream &buffer, std::vector<Transaction> &transactions, int blockSize, std::vector<User> &users)
{
    Block minedBlock;
    std::vector<std::vector<Transaction>> candidateBlocks;

    int maxAttempts = 1000;

#pragma omp parallel num_threads(5) default(none) shared(candidateBlocks, buffer, transactions, blockSize, users)
    {
        std::vector<Transaction> txToBlock = transactionsToBlock(transactions, blockSize, buffer, users);
#pragma omp critical
        {
            candidateBlocks.push_back(txToBlock);
        }
    }
    minedBlock = mineBlock(candidateBlocks, maxAttempts, previousBlockHash, difficulty, users, transactions, buffer);
    return minedBlock;
}

void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty, std::vector<User> &users)
{
    std::vector<Block> blockchain;
    std::stringstream buffer;

    while (!transactions.empty())
    {
        std::string previousBlockHash;
        Block newBlock;

        if (blockchain.empty())
            previousBlockHash.append(64, '0');
        else
            previousBlockHash = blockchain.back().getBlockHash();
        newBlock = parallelMining(previousBlockHash, difficulty, buffer, transactions, blockSize, users);
        blockchain.push_back(newBlock);

        std::cout << "Block #" << blockchain.size() << std::endl;
        buffer << "Block #" << blockchain.size() << std::endl;

        std::cout << newBlock << std::endl;
        buffer << newBlock << std::endl;

        std::ofstream fout;
        fout.open("logs/mining-log.txt");
        fout << buffer.str();
        fout.close();
    }
}