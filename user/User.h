#ifndef USER_H
#define USER_H

#include "../include/utxo.h"
#include <vector>
class User
{
private:
    std::string name;
    std::string publicKey;
    double balance;
    std::vector<UTXO> utxos;

public:
    User(std::string name, std::string publicKey);

    void setName(std::string name);
    std::string getName() const;

    void setPublicKey(std::string publicKey);
    std::string getPublicKey() const;

    void setBalance(double balance);
    double getBalance() const;

    void addUTXO(double amount);
    void removeUTXO(std::string id);
    std::vector<UTXO> getUTXOs();
};

#endif