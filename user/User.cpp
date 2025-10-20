#include "User.h"

User::User(std::string name, std::string publicKey, double balance)
{
    this.name = name;
    this.publicKey = publicKey;
    this.balance = balance;
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
