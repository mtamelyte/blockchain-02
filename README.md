<h1>Simplified blockchain implementation</h1>
<h2>About</h2>
<p>This program simulates a simplified version of a blockchain. It generates a set amount of users and transactions between those users, then mines blocks by picking a certain amount of transactions from the generated list and using a hashing function I made with the help of Claude AI to mine it and add it to the blockchain.</p>
<h2>UTXO model</h2>
<p>This implementation uses the UTXO model, so in all the transactions, payment is done through the creation and destruction of UTXOs. When the transaction is first generated, the UTXOs aren't added or deducted from users' wallets yet - they're only flagged as inputs or generated as outputs. They are only added to users' wallets or destroyed when the transaction is included in a block and added to the blockchain Each user has 50 UTXOs that are worth between 50 and 500 to begin with. This number was initially diffent (10 UTXOs worth between 100 and 1000), but because </p>
<h2>Step-by-step overview</h2>
<h3>Generating users</h3>
<p>The program generates 1 000 users, each of them have a username ("User_" and their number) and a public key, which is made by hashing their username, combined with a randomly generated salt. Then each of them gets 50 UTXOs with random values between 50 and 500 to spend in the transactions.</p>
<h3>Generating transactions</h3>
<p>The program then generates 10 000 transactions - it randomly picks two users from the list, checks that they are not the same person and that the sender has money to spend. A random amount to transfer is then randomly generated (but it is ensured that this amount is not larger than the sender's balance. In each transaction, the program goes over the UTXOs that the sender has, flags them as used in this transaction and then creates an equivalent UTXO in the transaction output list (but doesn't append it to the receiver yet). This goes on until it processes an UTXO that has the same value as the amount left to pay, at which point it ends the transaction, or has a value that is greater than the amount left to pay, which would cause it to generate two outputs - one for the receiver and one for the sender as change, before also ending the transaction. </p>
<h3>Mining blocks</h3>
<p></p>
