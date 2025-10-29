#include <vector>
#include "utxo.h"
#include "../block/Block.h"
#include "../transaction/Transaction.h"
#include "../user/User.h"
#include <chrono>
#include <random>
#include <iostream>
#include <list>
#include <sstream>
#include <iomanip>
#include <array>
#include <algorithm>

std::vector<Transaction> generateTransactions(const int txAmount, std::vector<User> &users);
void createBlockchain(std::vector<Transaction> &transactions, int blockSize, int difficulty, std::vector<User> users);
std::vector<User> generateUsers(int userAmount);
std::string generateSalt();
std::string hash(const std::string &input);
