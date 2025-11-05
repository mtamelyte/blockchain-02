#include "MerkleTree.h"

MerkleTree::MerkleTree(std::vector<Transaction> &transactions)
{
    std::vector<Node *> leaves;
    for (auto tx : transactions)
    {
        leaves.push_back(new Node(hash(tx.getTransactionId())));
    }

    this->root = buildTree(leaves);
}

MerkleTree::Node::Node(const std::string &h, Node *left, Node *right)
{
    this->value = h;
    this->left = left;
    this->right = right;
}

MerkleTree::Node *MerkleTree::buildTree(std::vector<Node *> &data)
{
    std::vector<Node *> newData;
    int leafAmt = data.size();

    if (data.size() % 2 != 0)
    {
        leafAmt--;
    }

    for (int i = 0; i < leafAmt; i += 2)
    {
        std::string dataToHash = data[i]->value + data[i + 1]->value;
        std::string hashedData = hash(dataToHash);
        newData.push_back(new Node(hashedData, data[i], data[i + 1]));
    }

    if (data.size() % 2 != 0)
    {
        std::string hashedData = hash(data.back()->value);
        newData.push_back(new Node(hashedData, data.back(), nullptr));
    }

    if (newData.size() == 1)
        return newData[0];
    else
        return buildTree(newData);
}

std::string MerkleTree::getRoot()
{
    return this->root->value;
}