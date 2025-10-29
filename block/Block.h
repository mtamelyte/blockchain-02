#ifndef BLOCK_H
#define BLOCK_H

#include "../include/lib.h"
class Block
{
private:
    std::string previousBlockHash;
    time_t timestamp;
    int version;
    std::string merkleRootHash;
    size_t nonce;
    int difficulty;

    std::vector<Transaction> transactions;

    std::string blockHash = "";

public:
    Block(std::string previousBlockHash, std::string merkleRootHash, size_t nonce, int difficulty);

    std::string getBlockHash();
    std::string calculateBlockHash();

    void setTransactions(std::vector<Transaction> transactions);

    size_t getNonce();
};

#endif