#include "include/lib.h"

std::string generateSalt()
{
    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> utfCode(33, 126);

    std::string salt = "";

    for (int i = 0; i < 10; i++)
    {
        salt += (char)utfCode(mt); // generate a random UTF-8 code and change it to a corresponding symbol
    }

    return salt;
}

std::vector<User> generateUsers(int userAmount)
{
    std::vector<User> users;
    for (int i = 1; i <= userAmount; i++)
    {
        std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<> amt(50, 500);

        const std::string name = "User_" + std::to_string(i); // create a username like "User_45"

        // create a public key
        const std::string preHashPublicKey = "User_" + std::to_string(i) + generateSalt();
        const std::string publicKey = hash(preHashPublicKey);

        User user(name, publicKey); // construct a user with the specified username and public key
        std::cout << name << " " << std::endl;
        std::cout << "UTXOs: " << std::endl;

        // generate 50 UTXOs for each user
        for (int j = 0; j < 15; j++)
        {
            int amount = amt(mt);
            UTXO utxo(user.getName(), user.getPublicKey(), amount, "", 0);
            user.addUTXO(utxo);
            std::cout << utxo.id << " " << utxo.amount << std::endl;
        }
        std::cout << std::endl;
        users.push_back(user);
    }

    return users;
}

std::vector<Transaction> generateTransactions(const int txAmount, std::vector<User> &users)
{
    std::vector<Transaction> transactions;

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

        // ensure the sender and the receiver aren't the same person and the sender has money they can transfer
        while (senderIt == receiverIt || users[senderIt].getBalance() == 0)
        {
            // if they are, regenerate the sender
            senderIt = num(mt);
        }

        // get the sender and receiver as objects
        User sender = users[senderIt];
        User receiver = users[receiverIt];

        // randomly generate an amount to transfer
        double amountToTransfer = amt(mt);

        // ensure the sender has enough for the transfer
        while (amountToTransfer > sender.getBalance())
        {
            // if they don't, pick a new amount to transfer
            amountToTransfer = amt(mt);
        }

        // get the existing UTXOs
        std::vector<UTXO> utxos = sender.getUTXOs();
        double remainingAmount = amountToTransfer;

        // create transaction id - a hash of all other data
        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);

        std::vector<UTXO> output;

        std::cout << "Transaction #" << i + 1 << std::endl;
        std::cout << "Sender: " << sender.getName() << ", receiver: " << receiver.getName() << ". Amount transferred: " << amountToTransfer << std::endl;
        std::cout << "Inputs: " << std::endl;

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

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
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

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
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

                std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;
                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), remainingAmount, txId, 0);
                UTXO change(sender.getName(), sender.getPublicKey(), utxo.amount - remainingAmount, txId, 1);

                output.push_back(newUtxo);
                output.push_back(change);

                remainingAmount = 0;
                break;
            }
        }

        std::cout << "Outputs: " << std::endl;
        buffer << "Outputs:" << std::endl;
        for (auto utxo : output)
        {
            std::cout << "UTXO ID: " << utxo.id << ", value: " << utxo.amount;
            buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount;
            if (utxo.changeFlag)
            {
                std::cout << " (change)";
                buffer << " (change)";
            }
            std::cout << std::endl;
            buffer << std::endl;
        }

        // update the utxo status in the sender's data
        sender.setUTXOs(utxos);

        // construct a transaction with all the generated data and add it to the list
        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer, output);
        transactions.push_back(tx);

        // ensure the changes are updated in the list of users as well
        users[senderIt] = sender;
        users[receiverIt] = receiver;

        std::cout << std::endl;
        buffer << std::endl;
    }

    std::ofstream fout;
    fout.open("transaction-log.txt");
    fout << buffer.str();
    fout.close();

    return transactions;
}