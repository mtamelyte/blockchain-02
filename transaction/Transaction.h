#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
class Transaction
{
private:
    std::string transactionId;
    std::string senderId;
    std::string receiverId;
    double amount;

public:
    Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount);

    std::string getTransactionId();
};

#endif