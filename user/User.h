#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../include/utxo.h"
class User
{
private:
    std::string name;
    std::string publicKey;
    std::unordered_map<std::string, UTXO> utxos;

public:
    User(std::string name, std::string publicKey);

    void setName(std::string name);
    std::string getName() const;

    void setPublicKey(std::string publicKey);
    std::string getPublicKey() const;

    double getBalance() const;

    void addUTXO(std::string id, UTXO utxo);
    void removeUTXO(std::string id);

    std::unordered_map<std::string, UTXO> getUTXOs();
    void setUTXOs(std::unordered_map<std::string, UTXO> utxos);

    friend std::ostream &operator<<(std::ostream &os, User &user);
};

#endif