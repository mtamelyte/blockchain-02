#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include <unordered_map>
#include "../include/utxo.h"
class Transaction
{
private:
    std::string transactionId;
    std::string senderId;
    std::string receiverId;
    double amount;
    std::unordered_map<std::string, UTXO> outputs;
    std::vector<std::string> inputs;

public:
    Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount, std::vector<std::string> inputs, std::unordered_map<std::string, UTXO> outputs);

    std::string getTransactionId() const;
    std::unordered_map<std::string, UTXO> getOutputs();
    std::vector<std::string> getInputs();
    std::string getSenderId();
    std::string getReceiverId();
    double getAmount();
};

#endif