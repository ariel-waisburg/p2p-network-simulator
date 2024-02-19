#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <vector>
#include <map>
#include "node.hpp"
#include "task.hpp"
#include <stdbool.h>
#include <set>

using namespace std;

class Block; // Forward declaration
class Node;
class Task;

class TXN {
    public:
        int txn_id;
        int sender_id;
        int receiver_id;
        int amount;
        int sender_bal;
        bool coinbase;
        TXN() : coinbase(false) {}
};


TXN createTransaction(Node miner, int id, long n_peers);

TXN createCoinbaseTransaction(int id, int txnId);

bool verifyTransactions(Block block);

double generateExponential(double mean);

map<int, Task> prepareTasksForTxnCrt(long n_peers);

#endif
