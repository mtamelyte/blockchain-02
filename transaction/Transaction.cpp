#include "Transaction.h"

Transaction::Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount, std::vector<std::string> inputs, std::unordered_map<std::string, UTXO> outputs)
{
    this->transactionId = transactionId;
    this->senderId = senderId;
    this->receiverId = receiverId;
    this->amount = amount;
    this->outputs = outputs;
    this->inputs = inputs;
}

std::string Transaction::getTransactionId() const
{
    return this->transactionId;
}

std::unordered_map<std::string, UTXO> Transaction::getOutputs()
{
    return this->outputs;
}

std::vector<std::string> Transaction::getInputs()
{
    return this->inputs;
}

std::string Transaction::getSenderId()
{
    return this->senderId;
}

std::string Transaction::getReceiverId()
{
    return this->receiverId;
}

double Transaction::getAmount()
{
    return amount;
}
