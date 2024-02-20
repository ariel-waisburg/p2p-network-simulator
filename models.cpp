#ifndef MODELS_HPP
#define MODELS_HPP

#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;

// Class representing a transaction
class Txn
{
public:
    int txn_id;
    int sender_id;
    int receiver_id;
    int amount;
    int sender_bal;
    bool coinbase = false;
};

// Class representing a block in the blockchain
class Block
{
public:
    int blk_id; // 0 to ... (0 being for genesis)
    vector<Txn> txn_tree;
    int crt_time;
};

// Enum defining different types of tasks
enum task_type
{
    blk_crt, // Create block task
    blk_rcv, // Recieve block task
    txn_crt, // Transaction create task
    txn_rcv, // Transaction recieved task
};

// Class representing a task to be performed by a node
class Task
{
public:
    task_type type;
    int trigger_time;
    vector<Block> blockchain;
    Txn txn;
};

// Functor class to compare tasks for priority queue
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
        return false;
    }
};

// Class representing a node in the network
class Node
{
public:
    int peer_id;   // 0 to ...
    int cpu = 0;   // 0 or 1
    int speed = 0; // 0 or 1
    int amnt;
    double hashPower;
    bool blk_crt_pending = false;
    vector<int> peer_nbh;
    set<int> knownTxns;
    queue<Txn> validatedTxns;
    priority_queue<Task, vector<Task>, Compare> tasks;
    vector<Block> blockchain;
};

#endif