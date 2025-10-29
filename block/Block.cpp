#include "Block.h"
#include "../include/lib.h"

Block::Block(std::string previousBlockHash, std::string merkleRootHash, size_t nonce, int difficulty)
{
    this->previousBlockHash = previousBlockHash;
    this->version = 1;
    this->merkleRootHash = merkleRootHash;
    this->difficulty = difficulty;
    this->nonce = nonce;
    this->timestamp = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

std::string Block::getBlockHash()
{
    return blockHash;
}

std::string Block::calculateBlockHash()
{
    if (blockHash != "")
        return blockHash;
    std::string stringToHash = previousBlockHash + std::to_string(timestamp) + std::to_string(version) + merkleRootHash + std::to_string(nonce) + std::to_string(difficulty);
    blockHash = hash(stringToHash);
    return blockHash;
}

void Block::setTransactions(std::vector<Transaction> transactions)
{
    this->transactions = transactions;
}

size_t Block::getNonce()
{
    return this->nonce;
}
