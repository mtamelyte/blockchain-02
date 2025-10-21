#ifndef BLOCKHEADER_H
#define BLOCKHEADER_H

#include <string>

class BlockHeader
{
private:
    std::string previousBlockHash;
    time_t timestamp;
    int version;
    std::string allTransactionHash;
    size_t nonce;
    int difficulty;

public:
    BlockHeader() = default;
    BlockHeader(std::string previousBlockHash, time_t timestamp, int version, std::string allTransactionHash, size_t nonce, int difficulty);
};

#endif BLOCKHEADER_H