#include "task.hpp"
#include "node.hpp"
#include "transaction.hpp"

Task prepareTaskForBlockCreate(long time)
{
    Task task;
    task.type = blk_crt;
    task.trigger_time = time;
    return task;
}

Task prepareTaskForBlockRecieve(double time, vector<Block> blockchain)
{
    Task task;
    task.type = blk_rcv;
    task.trigger_time = time;
    task.blockchain = blockchain;
    return task;
}

Task prepareTaskForTxnRcv(long time, TXN txn)
{
    Task task;
    task.type = txn_rcv;
    task.trigger_time = time;
    task.txn = txn;
    return task;
}

Task prepareTaskForTxnCrt(long time)
{
    Task task;
    task.type = txn_crt;
    task.trigger_time = time;
    return task;
}

Task prepareTaskForPowDone(long time, Task rcvTask)
{
    Task task;
    task.type = pow_done;
    task.trigger_time = time;
    task.blockchain = rcvTask.blockchain;
    return task;
}
