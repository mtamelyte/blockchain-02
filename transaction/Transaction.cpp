#include "Transaction.h"

Transaction::Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount, std::vector<UTXO> inputs, std::vector<UTXO> ouputs)
{
    this->transactionId = transactionId;
    this->senderId = senderId;
    this->receiverId = receiverId;
    this->amount = amount;
    this->inputs = inputs;
    this->outputs = outputs;
}

std::string Transaction::getTransactionId()
{
    return this->transactionId;
}
