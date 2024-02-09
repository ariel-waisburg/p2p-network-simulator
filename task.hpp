#ifndef TASK_HPP
#define TASK_HPP

#include <vector>

using namespace std;

enum task_type
{
    blk_rcv,
    blk_snd,
    blk_crt,
    pow_start,
    pow_end
};

class Task
{
public:
    task_type type;
    long trigger_time;
};

Task prepareForBlockSend(long time);
Task prepareForBlockCreate(long time);

class Compare
{
public:
    bool operator()(Task below, Task above)
    {
        if (below.trigger_time > above.trigger_time)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

#endif