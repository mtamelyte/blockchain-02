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
    fout.open("logs/user-log.txt");
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

    std::stringstream buffer;

    std::mt19937 mt(static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> num(0, users.size() - 1);
    std::uniform_int_distribution<> amt(1, 1000);

    for (int i = 0; i < txAmount; i++)
    {
        int senderIt = num(mt);
        int receiverIt = num(mt);

        while (senderIt == receiverIt)
        {
            senderIt = num(mt);
        }

        User sender = users[senderIt];
        User receiver = users[receiverIt];

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

        sender.sortUTXOs();
        std::vector<UTXO> utxos = sender.getUTXOs();
        double remainingAmount = amountToTransfer;

        std::string idPreHash = sender.getPublicKey() + receiver.getPublicKey() + std::to_string(amountToTransfer);
        std::string txId = hash(idPreHash);

        std::vector<UTXO> output;

        buffer << "Transaction #" << i + 1 << std::endl;
        buffer << "Sender: " << sender.getName() << ", receiver: " << receiver.getName() << ". Amount transferred: " << amountToTransfer << std::endl;
        buffer << "Inputs: " << std::endl;

        for (auto &utxo : utxos)
        {
            if (utxo.used)
            {
                continue;
            }
            else if (utxo.amount < remainingAmount)
            {
                utxo.used = true;
                utxo.txId = txId;

                buffer << "UTXO ID: " << utxo.id << ", value: " << utxo.amount << std::endl;

                UTXO newUtxo(receiver.getName(), receiver.getPublicKey(), utxo.amount, txId, 0);
                output.push_back(newUtxo);

                remainingAmount -= utxo.amount;
            }
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

        sender.setUTXOs(utxos);

        Transaction tx(txId, sender.getPublicKey(), receiver.getPublicKey(), amountToTransfer, output);
        transactions.push_back(tx);
        transactionsGenerated++;

        users[senderIt] = sender;
        users[receiverIt] = receiver;

        buffer << std::endl;
    }

    std::ofstream fout;
    fout.open("logs/transaction-log.txt");
    fout << buffer.str();
    fout.close();

    std::cout << transactionsGenerated << " valid transactions generated. Full log available in 'transaction-log.txt'." << std::endl;
    std::cout << std::endl;

    return transactions;
}