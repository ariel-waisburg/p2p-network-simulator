// task.hpp

#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include "latency.hpp"
#include "transaction.hpp"  // Include the header for TXN
#include "node.hpp"         // Include the header for Block

class Block; // Forward declaration
class TXN;

using namespace std;

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
    long trigger_time;
    vector<Block> blockchain;
    TXN txn;
};

Task prepareTaskForBlockCreate(long time);
Task prepareTaskForBlockRecieve(long time, vector<Block> blockchain);
Task prepareTaskForTxnRcv(long time, TXN txn);
Task prepareTaskForTxnCrt(long time);
Task prepareTaskForPowDone(long time, Task rcvTask);

class Compare
{
public:
    bool operator()(Task below, Task above);
    bool compareType(task_type below, task_type above);
};

#endif
