#ifndef TASK_HPP
#define TASK_HPP

#include <vector>

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
    bool operator()(Task below, Task above)
    {
        if (below.trigger_time > above.trigger_time)
        {
            return true;
        }
        else if (below.trigger_time == above.trigger_time){
            return compareType(below.type, above.type); // Comparing type of tasks for race cases.
        }
        else {
            return false;
        }
    }

    bool compareType(task_type below, task_type above){
        if(below == blk_crt && above == blk_rcv) return true;
        if(below == txn_rcv && above == txn_crt) return true;
        if(below == blk_crt && above == pow_done) return true;
        return false;
    }
};

#endif