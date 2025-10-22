#include <bits/stdc++.h>
#include <chrono>
#include "../user/User.h"
#include "../transaction/Transaction.h"
#include "../block/Block.h"

std::string hash(const std::string &input);
std::vector<User> generateUsers(double userAmount);
std::vector<Transaction> generateTransactions(int txAmount, std::vector<User> &users);
std::string generateSalt();
void createBlockchain(std::vector<Transaction> &transactions, int blockSize);
