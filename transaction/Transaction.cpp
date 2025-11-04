#include "Transaction.h"

Transaction::Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount, std::vector<UTXO> ouputs)
{
    this->transactionId = transactionId;
    this->senderId = senderId;
    this->receiverId = receiverId;
    this->amount = amount;
    this->outputs = outputs;
}

std::string Transaction::getTransactionId()
{
    return this->transactionId;
}

std::vector<UTXO> Transaction::getOutputs()
{
    return this->outputs;
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
