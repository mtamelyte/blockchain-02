#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include "../include/utxo.h"
class Transaction
{
private:
    std::string transactionId;
    std::string senderId;
    std::string receiverId;
    double amount;
    std::vector<UTXO> inputs;
    std::vector<UTXO> outputs;

public:
    Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount, std::vector<UTXO> inputs, std::vector<UTXO> ouputs);

    std::string getTransactionId();
};

#endif