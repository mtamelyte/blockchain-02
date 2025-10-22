#include <bits/stdc++.h>
#include "../transaction/Transaction.h"
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>

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
};

#endif