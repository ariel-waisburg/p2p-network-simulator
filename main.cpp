#include <iostream>
#include <queue>
#include <stack>
#include "node.hpp"
#define Pll pair<long, long>
using namespace std;

long global_time = 0;

// Will separate the logic for manager is needed.
void manager(long time_limit, vector<Node> miners) {}

int main()
{
    long n_peers;
    long time_limit;

    cout << "\nEnter the number of peers in the network: ";
    cin >> n_peers;
    cout << "\nDuration in seconds for the simulation to run: ";
    cin >> time_limit;

    vector<Node> miners = initialization();

    while (global_time < time_limit)
    {
        priority_queue<Pll, vector<Pll>, greater<Pll>> miner_idx;
        for (long i = 0; i < n_peers; i++)
        {
            if (!miners[i].blk_crt_pending)
            {
                miners[i].tasks.push(prepareForBlockCreate(5)); // Must be a random time for generation of block
                miners[i].blk_crt_pending = true;
            }
            miner_idx.push({miners[i].tasks.top().trigger_time, i}); // TODO: loop for all the tasks with same target time for optimization - not important
        }

        long smallest_time = miner_idx.top().first;

        while (!miner_idx.empty())
        {
            long current_task_time = miner_idx.top().first;
            if (current_task_time > smallest_time)
                break;

            long idx = miner_idx.top().second;
            Task task = miners[idx].tasks.top();

            /* Do whatever operation you have to do with task.
            Just make sure to invert blk_crt_pending to false if
            the current task was to create a block itself.

            Also if any events like sending a block to some other
            nodes happnes then introduce task with correct time in
            that other miners task list */

            miners[idx].tasks.pop();
            miner_idx.pop();
        }
        global_time += smallest_time;
    }

    cout << "\nSimulation ended at time " << global_time << " seconds\n";
    return 0;
}

/*
main.o: main.cpp node.hpp transaction.hpp
    ${CC} ${CFLAGS} -c main.cpp
*/