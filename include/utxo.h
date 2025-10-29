#ifndef UTXO_H
#define UTXO_H

#include <string>

std::string generateSalt();
std::string hash(const std::string &input);
class UTXO
{
public:
    std::string id;
    double amount;
    bool used;
    std::string txId;
    bool changeFlag;

    UTXO(std::string userName, std::string userKey, int amount, std::string txId, bool changeFlag)
    {
        std::string idToHash = userName + userKey + std::to_string(amount) + generateSalt();
        std::string id = hash(idToHash);

        this->id = id;
        this->amount = amount;
        this->used = false;
        this->txId = txId;
        this->changeFlag = changeFlag;
    }
};

#endif