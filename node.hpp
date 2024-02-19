#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <queue>
#include "transaction.hpp"
#include "task.hpp"
#include <set>

class Task; // Forward declaration
class Compare; // Forward declaration

using namespace std;

class Node
{
public:
    int peer_id; // 0 to ...
    int cpu;     // 0 or 1
    int speed;   // 0 or 1
    int amnt;
    bool blk_crt_pending = false;
    vector<long> peer_nbh;
    set<int> knownTxns;
    vector<TXN> validatedTxns;
    priority_queue<Task, vector<Task>, Compare> tasks;
    vector<Block> blockchain;
};

class Block
{
public:
    int blk_id; // 0 to ...
    vector<TXN> txn_tree;
    int crt_time;
};

Block prepareNewBlock(int id, int crt_time);

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers);

int getBalance(vector<Node> p, int peer_id, int n_peers);

vector<Node> initialization(long numPeers, long global_time);

#endif
