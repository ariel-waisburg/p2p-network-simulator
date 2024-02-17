#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include <set>
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
TXN createTransaction(Node miner, int id, long n_peers)
{
    TXN txn;

    srand(static_cast<unsigned>(time(0)));        // Seed for random number generation
    int receiver_id = generateRandom(0, n_peers); // Select a random receiver ID
    while (receiver_id == miner.peer_id)
        receiver_id = generateRandom(0, n_peers);

    txn.amount = generateRandom(1, 20); // Create a random amount between 1 to 20
    txn.sender_id = miner.peer_id;
    txn.sender_bal = miner.amnt;
    txn.receiver_id = receiver_id;
    txn.txn_id = id;

    cout << " TxnID: " << txn.txn_id << " " << txn.sender_id << " pays " << txn.receiver_id << " " << txn.amount << " coins" << endl;

    return txn;
}

TXN createCoinbaseTransaction(int id, int txnId)
{
    TXN txn;
    txn.coinbase = true;
    txn.sender_id = -1; // Sender is not there for coinbase
    txn.receiver_id = id;
    txn.amount = 50;
    txn.txn_id = txnId;
    return txn;
}

bool verifyTransactions(Block block)
{
    vector<TXN> txns = block.txn_tree;
    for (int i = 0; i < txns.size(); i++)
    {
        if (txns[i].amount > txns[i].sender_bal)
            return false;
    }
    return true;
}

map<int, Task> prepareTasksForTxnCrt(long n_peers)
{
    map<int, Task> tasks;
    set<int> ids;
    int taskCount = 0;
    while (taskCount < 10) // Generating random 10 transactions after every iteration
    {
        int senderId = generateRandom(0, n_peers);
        while (ids.find(senderId) != ids.end())
            senderId = generateRandom(0, n_peers);
        Task task = prepareTaskForTxnCrt(5);
        tasks[senderId] = task;
        taskCount++;
    }
    return tasks;
}
