#include "BlockHeader.h"

BlockHeader::BlockHeader(std::string previousBlockHash, time_t timestamp, int version, std::string allTransactionHash, size_t nonce, int difficulty)
{
    this->previousBlockHash = previousBlockHash;
    this->timestamp = timestamp;
    this->version = version;
    this->allTransactionHash = allTransactionHash;
    this->difficulty = difficulty;
    this->nonce = nonce;
}