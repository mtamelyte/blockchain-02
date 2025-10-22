#include "Transaction.h"

Transaction::Transaction(std::string transactionId, std::string senderId, std::string receiverId, double amount)
{
    this->transactionId = transactionId;
    this->senderId = senderId;
    this->receiverId = receiverId;
    this->amount = amount;
}

std::string Transaction::getTransactionId()
{
    return this->transactionId;
}
