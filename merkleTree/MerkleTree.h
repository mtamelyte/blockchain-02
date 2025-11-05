#ifndef MERKLETREE_H
#define MERKLETREE_H

#include "../include/lib.h"

class MerkleTree
{
public:
    struct Node
    {
        std::string value;
        struct Node *left;
        struct Node *right;
        Node *node;

        Node(const std::string &h) : value(h), left(nullptr), right(nullptr) {};
        Node(const std::string &h, Node *left, Node *right);

        Node *getnode()
        {
            return node;
        }
    };

    MerkleTree(std::vector<Transaction> &transactions);
    Node *buildTree(std::vector<Node *> &data);
    std::string getRoot();

private:
    int treeHeight;
    Node *root;
};

#endif