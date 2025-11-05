#ifndef UTXO_H
#define UTXO_H

#include <string>

std::string generateSalt();
std::string hash(const std::string &input);
class UTXO
{
public:
    double amount;
    bool used;
    bool changeFlag;

    UTXO(int amount, bool changeFlag)
    {
        this->amount = amount;
        this->used = false;
        this->changeFlag = changeFlag;
    }
};

#endif