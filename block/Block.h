#include "BlockHeader.h"
#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>

class Block
{
private:
    BlockHeader blockheader;
    std::vector<std::string> txIds;

public:
    Block(BlockHeader blockHeader, std::vector<std::string> txIds);
}