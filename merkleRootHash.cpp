#include "include/lib.h"

bool isOdd(int number)
{
    if (number % 2 == 0)
        return false;
    return true;
}

std::string merkleRootHash(std::vector<Transaction> transactions)
{
    int length = transactions.size();
    std::vector<std::string> hashes;

    if (isOdd(transactions.size()))
        length--;

    for (int i = 0; i < length; i += 2)
    {
        std::string txToHash = transactions[i].getTransactionId() + transactions[i + 1].getTransactionId();
        hashes.push_back(hash(txToHash));
    }

    if (isOdd(transactions.size()))
        hashes.push_back(hash(transactions.back().getTransactionId()));

    while (hashes.size() != 1)
    {
        int length = hashes.size();
        int counter = 0;
        std::string finalTx = "";

        if (isOdd(hashes.size()))
        {
            length--;
            finalTx = hashes.back();
        }

        for (int i = 0; i < length; i += 2)
        {
            std::string txToHash = hashes[i] + hashes[i + 1];
            hashes[counter] = hash(txToHash);
            counter++;
        }

        if (finalTx != "")
        {
            hashes[counter] = hash(finalTx);
            counter++;
            finalTx = "";
        }

        hashes.resize(counter);
    }

    return hashes[0];
}