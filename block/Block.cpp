#include "Block.h"

Block::Block(BlockHeader blockHeader, std::vector<std::string> txIds)
{
    this->blockheader = blockHeader;
    this->txIds = txIds;
}