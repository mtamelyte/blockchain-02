#ifndef USER_H
#define USER_H

#include <string>

class User
{
private:
    std::string name;
    std::string publicKey;
    double balance;

public:
    User(std::string name, std::string publicKey, double balance);

    void setName(std::string name);
    std::string getName() const;

    void setPublicKey(std::string publicKey);
    std::string getPublicKey() const;

    void setBalance(double balance);
    double getBalance() const;
};

#endif