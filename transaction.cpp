#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "node.hpp"
#include "transaction.hpp"

using namespace std;

// Function to generate a random integer within a given range
int generateRandom(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// Function to generate a random number from an exponential distribution
double generateExponential(double mean)
{
    double u = rand() / (RAND_MAX + 1.0);
    return -mean * log(1 - u);
}

// Function to create transactions between random peers with exponential interarrival times
vector<TXN> createTransaction(vector<Node> Peers, int n_peers, double mean_interarrival_time, int txn_id)
{
    vector<TXN> cur_txn;

    // Seed for random number generation
    srand(static_cast<unsigned>(time(0)));

    //double current_time = 0.0;

    // Sleep for exponential interarrival time
    //double interarrival_time = generateExponential(mean_interarrival_time);
    //current_time += interarrival_time;
    // todo : simulate the passage of time...

    // Select random sender and receiver IDs
    int sender_id = generateRandom(0, n_peers);
    int receiver_id = generateRandom(0, n_peers);

    // Ensure sender and receiver are different
    while (receiver_id == sender_id)
    {
        receiver_id = generateRandom(0, n_peers);
    }
    int transaction_amount = generateRandom(1, 20);
    // Check if the sender has enough money to perform the transaction
    if (getBalance(Peers, sender_id, n_peers) >= transaction_amount)
    {
        TXN txn;
        txn.txn_id = txn_id; // Assuming txn_id starts from 1
        txn.sender_id = sender_id;
        txn.receiver_id = receiver_id;
        txn.amount = transaction_amount;

        // Update balances after successful transaction
        updateBalance(Peers, sender_id, -txn.amount, n_peers);
        updateBalance(Peers, receiver_id, txn.amount, n_peers);

        cur_txn.push_back(txn);

        cout << " TxnID: " << txn.txn_id << " " << txn.sender_id << " pays " << txn.receiver_id << " " << txn.amount << " coins" << endl;

    }

    return cur_txn;
}
