#include "User.h"
#include "../include/lib.h"

User::User(std::string name, std::string publicKey)
{
    this->name = name;
    this->publicKey = publicKey;
    this->utxos = {};
}

void User::setName(std::string name)
{
    this->name = name;
}

std::string User::getName() const
{
    return this->name;
}

void User::setPublicKey(std::string publicKey)
{
    this->publicKey = publicKey;
}

std::string User::getPublicKey() const
{
    return this->publicKey;
}

double User::getBalance() const
{
    double balance = 0;
    for (auto utxo : utxos)
    {
        if (utxo.second.used == false)
            balance += utxo.second.amount;
    }
    return balance;
}

void User::addUTXO(std::string id, UTXO utxo)
{
    this->utxos.insert({id, utxo});
}

void User::removeUTXO(std::string id)
{
    utxos.erase(id);
}

std::unordered_map<std::string, UTXO> User::getUTXOs()
{
    return this->utxos;
}

void User::setUTXOs(std::unordered_map<std::string, UTXO> utxos)
{
    this->utxos = utxos;
}

std::ostream &operator<<(std::ostream &os, User &user)
{
    os << user.name << "\nPublic key: " << user.publicKey << "\nUTXOs: ";
    for (auto &utxo : user.utxos)
    {
        os << "\n"
           << utxo.first << " " << utxo.second.amount;
    }
    os << "\n";
    return os;
}
