#include "Block.h"
#include "../include/lib.h"

Block::Block(std::string previousBlockHash, bc::hash_digest merkleRootHash, size_t nonce, int difficulty)
{
    this->previousBlockHash = previousBlockHash;
    this->version = "v0.21";
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
    std::string stringToHash = previousBlockHash + std::to_string(timestamp) + version + bc::encode_base16(merkleRootHash) + std::to_string(nonce) + std::to_string(difficulty);
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

std::ostream &operator<<(std::ostream &os, Block &block)
{
    os << "Block hash: " << block.blockHash << "\nPrevious block hash: " << block.previousBlockHash << " \nTimestamp: " << block.timestamp << " \nVersion: " << block.version << " \nMerkle Root Hash: " << bc::encode_base16(block.merkleRootHash) << " \nNonce: " << block.nonce << " \nDifficulty: " << block.difficulty << "\n";
    return os;
}
