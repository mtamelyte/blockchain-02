#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "../include/utxo.h"
class User
{
private:
    std::string name;
    std::string publicKey;
    std::vector<UTXO> utxos;

public:
    User(std::string name, std::string publicKey);

    void setName(std::string name);
    std::string getName() const;

    void setPublicKey(std::string publicKey);
    std::string getPublicKey() const;

    double getBalance() const;

    void addUTXO(UTXO utxo);
    void removeUTXO(std::string id);
    std::vector<UTXO> getUTXOs();
};

#endif