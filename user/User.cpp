#include "User.h"
#include "../include/lib.h"

User::User(std::string name, std::string publicKey)
{
    this->name = name;
    this->publicKey = publicKey;
    this->balance = 0;
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

void User::setBalance(double balance)
{
    this->balance = balance;
}

double User::getBalance() const
{
    return this->balance;
}

void User::addUTXO(double amount)
{

    std::string idToHash = this->name + this->publicKey + std::to_string(amount) + generateSalt();
    std::string id = hash(idToHash);

    UTXO utxo;
    utxo.id = id;
    utxo.amount = amount;

    this->balance += amount;

    this->utxos.push_back(utxo);
}

void User::removeUTXO(std::string id)
{
    std::vector<UTXO>::iterator it = utxos.begin();
    for (auto utxo : utxos)
    {
        if (utxo.id == id)
        {
            this->balance -= utxo.amount;
            break;
        }
        it++;
    }
    utxos.erase(it);
}

std::vector<UTXO> User::getUTXOs()
{
    return this->utxos;
}
