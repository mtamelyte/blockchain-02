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
    for (UTXO utxo : utxos)
    {
        if (utxo.used == false)
            balance += utxo.amount;
    }
    return balance;
}

void User::addUTXO(UTXO utxo)
{
    this->utxos.push_back(utxo);
}

void User::removeUTXO(std::string id)
{
    std::vector<UTXO>::iterator it = utxos.begin();
    for (auto utxo : utxos)
    {
        if (utxo.id == id)
        {
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

void User::setUTXOs(std::vector<UTXO> utxos)
{
    this->utxos = utxos;
}

void User::sortUTXOs()
{
    std::sort(this->utxos.begin(), this->utxos.end(), [](UTXO a, UTXO b)
              { return a.amount > b.amount; });
}

std::ostream &operator<<(std::ostream &os, User &user)
{
    os << user.name << "\nPublic key: " << user.publicKey << "\nUTXOs: ";
    for (UTXO &utxo : user.utxos)
    {
        os << "\n"
           << utxo.id << " " << utxo.amount;
    }
    os << "\n";
    return os;
}
