#ifndef MODELS_HPP
#define MODELS_HPP

#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;

class TXN
{
public:
    int txn_id;
    int sender_id;
    int receiver_id;
    int amount;
    int sender_bal;
    bool coinbase = false;
};

class Block
{
public:
    int blk_id; // 0 to ...
    vector<TXN> txn_tree;
    int crt_time;
};

enum task_type
{
    blk_rcv,
    blk_crt,
    txn_rcv,
    txn_crt,
    pow_done
};

class Task
{
public:
    task_type type;
    int trigger_time;
    vector<Block> blockchain;
    TXN txn;
};

class Compare
{
public:
    bool operator()(Task below, Task above)
    {
        if (below.trigger_time > above.trigger_time)
        {
            return true;
        }
        else if (below.trigger_time == above.trigger_time)
        {
            return compareType(below.type, above.type); // Comparing type of tasks for race cases.
        }
        else
        {
            return false;
        }
    }

    bool compareType(task_type below, task_type above)
    {
        if (below == blk_crt && above == blk_rcv)
            return true;
        if (below == txn_rcv && above == txn_crt)
            return true;
        if (below == blk_crt && above == pow_done)
            return true;
        return false;
    }
};

class Node
{
public:
    int peer_id; // 0 to ...
    int cpu;     // 0 or 1
    int speed;   // 0 or 1
    int amnt;
    bool blk_crt_pending = false;
    vector<int> peer_nbh;
    set<int> knownTxns;
    vector<TXN> validatedTxns;
    priority_queue<Task, vector<Task>, Compare> tasks;
    vector<Block> blockchain;
};

#endif