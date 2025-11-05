#include "../include/lib.h"

std::string generateSalt()
{
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> utfCode(33, 126);

    std::string salt = "";

    for (int i = 0; i < 10; i++)
    {
        salt += (char)utfCode(mt);
    }

    return salt;
}

std::vector<User> generateUsers(int userAmount)
{
    std::vector<User> users;
    int usersGenerated = 0;

    std::stringstream buffer;

    for (int i = 1; i <= userAmount; i++)
    {
        std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> amt(50, 500);

        const std::string name = "User_" + std::to_string(i);

        const std::string preHashPublicKey = "User_" + std::to_string(i) + generateSalt();
        const std::string publicKey = hash(preHashPublicKey);

        User user(name, publicKey);
        // generate 50 UTXOs for each user
        for (int j = 0; j < 50; j++)
        {
            int amount = amt(mt);
            UTXO utxo(user.getName(), user.getPublicKey(), amount, "", 0);
            user.addUTXO(utxo);
        }
        users.push_back(user);
        buffer << user << std::endl;
        usersGenerated++;
    }

    std::ofstream fout;
    fout.open("user-log.txt");
    fout << buffer.str();
    fout.close();

    std::cout << usersGenerated << " users generated. Full log available in 'user-log.txt'" << std::endl;
    std::cout << std::endl;

    return users;
}

std::vector<Transaction> generateTransactions(const int txAmount, std::vector<User> &users)
{
    std::vector<Transaction> transactions;
    int transactionsGenerated = 0;

    // prepare for writing to file
    std::stringstream buffer;

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> num(0, users.size() - 1);
    std::uniform_int_distribution<> amt(1, 1000);

    for (int i = 0; i < txAmount; i++)
    {
        // randomly pick a sender and a receiver
        int senderIt = num(mt);
        int receiverIt = num(mt);

        // ensure the sender and the receiver aren't the same person
        while (senderIt == receiverIt)
        {
            senderIt = num(mt);
        }

        // get the sender and receiver as objects
        User sender = users[senderIt];
        User receiver = users[receiverIt];

        // randomly generate an amount to transfer
        double amountToTransfer = amt(mt);

        if (sender.getBalance() < amountToTransfer)
        {
            buffer << "Transaction #" << i + 1 << std::endl;
            buffer << sender.getName() << " does not have enough for the transaction." << std::endl;
            buffer << "The amount to transfer is " << amountToTransfer << ", while " << sender.getName() << " only has " << sender.getBalance() << std::endl;
            buffer << "Transaction skipped." << std::endl;
            buffer << std::endl;
            continue;
        }

        // get the existing UTXOs
        sender.sortUTXOs();
        std::vector<UTXO> utxos = sender.getUTXOs();
        double remainingAmount = amountToTransfer;

        // create transaction id - a hash of all other data
        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);

        std::vector<UTXO> output;

        buffer << "Transaction #" << i + 1 << std::endl;
        buffer << "Sender: " << sender.getName() << ", receiver: " << receiver.getName() << ". Amount transferred: " << amountToTransfer << std::endl;
        buffer << "Inputs: " << std::endl;

        for (auto &utxo : utxos)
        {
            // if the current utxo is already used in a different transaction, move on
            if (utxo.used)
            {
                continue;
            }
            // if the current utxo is not enough to close the transaction, move it to the receiver, deduct the amount and keep going
            else if (utxo.amount < remainingAmount)
            {
                utxo.used = true;
                utxo.txId = txId;

                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount, txId, 0);
                output.push_back(newUtxo);

                remainingAmount -= utxo.amount;
            }

            // if the current utxo is exactly enough, move it to the receiver and break the loop
            else if (utxo.amount == remainingAmount)
            {
                utxo.used = true;
                utxo.txId = txId;

                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount, txId, 0);
                output.push_back(newUtxo);

                remainingAmount = 0;
                break;
            }

            // if the current utxo is bigger than the remaining amount, remove it, split it into the needed sum and the remainder and add them back to respective users
            else
            {
                utxo.used = true;
                utxo.txId = txId;

                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), remainingAmount, txId, 0);
                UTXO change(sender.getName(), sender.getPublicKey(), utxo.amount - remainingAmount, txId, 1);

                output.push_back(newUtxo);
                output.push_back(change);

                remainingAmount = 0;
                break;
            }
        }

        buffer << "Outputs:" << std::endl;
        for (auto utxo : output)
        {
            buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount;
            if (utxo.changeFlag)
            {
                buffer << " (change)";
            }
            buffer << std::endl;
        }

        // update the utxo status in the sender's data
        sender.setUTXOs(utxos);

        // construct a transaction with all the generated data and add it to the list
        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer, output);
        transactions.push_back(tx);
        transactionsGenerated++;

        // ensure the changes are updated in the list of users as well
        users[senderIt] = sender;
        users[receiverIt] = receiver;

        buffer << std::endl;
    }

    std::ofstream fout;
    fout.open("transaction-log.txt");
    fout << buffer.str();
    fout.close();

    std::cout << transactionsGenerated << " valid transactions generated. Full log available in 'transaction-log.txt'." << std::endl;
    std::cout << std::endl;

    return transactions;
}