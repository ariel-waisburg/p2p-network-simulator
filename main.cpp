#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <random>
#include <fstream>
#include "models.cpp"
#include "functions.cpp"
#include "visualization.cpp"
#define Pll pair<int, int>
using namespace std;

// Current time of simulation in ms
long global_time = 0;

// Will separate the logic for manager is needed.
void manager(vector<Node> &miners, long time_limit, double lambda, mt19937 &gen) {
    long n_peers = miners.size();
    set<int> txnSet; // Global set to see any used txns behaving as UTXO
    int blkId = 1;   // Unique Id for blocks created in increasing format
    int txnId = 1;   // Unique Id for transactions created in increasing format
    double prop_delay = generatePropDelay(gen);

    while (global_time < time_limit)
    {
        priority_queue<Pll, vector<Pll>, greater<Pll > > miner_idx;
        for (int i = 0; i < n_peers; i++)
        {
            if (!miners[i].blk_crt_pending)
            {
                miners[i].tasks.push(prepareTaskForBlockCreate(generateExponential(lambda) * 1000)); // TODO - Use better randomization
                miners[i].blk_crt_pending = true;
            }
            miner_idx.push(make_pair(miners[i].tasks.top().trigger_time, i));
        }

        int smallest_time = miner_idx.top().first;

        while (!miner_idx.empty())
        {
            int current_task_time = miner_idx.top().first;
            if (current_task_time > smallest_time)
                break;

            int idx = miner_idx.top().second;
            Task task = miners[idx].tasks.top();
            vector<int> neighbours = miners[idx].peer_nbh;
            vector<TXN> txns = miners[idx].validatedTxns;

            /* Do whatever operation you have to do with task.
            Just make sure to invert blk_crt_pending to false if
            the current task was to create a block itself.

            Also if any events like sending a block to some other
            nodes happnes then introduce task with correct time in
            that other miners task list */

            switch (task.type)
            {
            case blk_crt:
            {
                Block newBlock = prepareNewBlock(blkId++, global_time + smallest_time);
                TXN reward = createCoinbaseTransaction(miners[idx].peer_id, txnId++);
                newBlock.txn_tree.push_back(reward);
                txnSet.insert(reward.txn_id);

                // Pick all the valid transactions that are not yet used
                for (int i = 0; i < txns.size(); i++)
                {
                    if (txnSet.find(txns[i].txn_id) == txnSet.end())
                    {
                        txnSet.insert(txns[i].txn_id);
                        newBlock.txn_tree.push_back(txns[i]);
                        if (newBlock.txn_tree.size() == 1024)
                        { // break after 1MB
                            miners[idx].validatedTxns.erase(miners[idx].validatedTxns.begin());
                            break;
                        }
                    }
                    miners[idx].validatedTxns.erase(miners[idx].validatedTxns.begin());
                }

                miners[idx].blockchain.push_back(newBlock);
                for (int i = 0; i < neighbours.size(); i++)
                {
                    Task rcvTask = prepareTaskForBlockRecieve(latency(miners[idx], miners[neighbours[i]], 'b', prop_delay, gen) * 1000, miners[idx].blockchain); // Use Ariel's latency code here to determine time
                    miners[neighbours[i]].tasks.push(rcvTask);
                }
                miners[idx].amnt += 50; // Incremented by 50 coins for coinbase transaction
                miners[idx].blk_crt_pending = false;
            }
            break;
            case blk_rcv:
            {
                if (task.blockchain.size() > miners[idx].blockchain.size())
                {
                    Task powTask = prepareTaskForPowDone(generateExponential(lambda) * 1000, task);
                    miners[idx].tasks.push(powTask);
                }
            }
            break;
            case pow_done:
            {
                int j = 0;
                for (int i = 0; i < miners[idx].blockchain.size(); i++)
                {
                    if (miners[idx].blockchain[i].blk_id == task.blockchain[i].blk_id)
                    {
                        j++;
                    }
                    else
                    {
                        break;
                    }
                }

                bool allCorrect = true;
                for (int i = j; i < task.blockchain.size(); i++)
                {
                    // Check if the receieved blockchain has valid transactions
                    if (!verifyTransactions(task.blockchain[i]))
                    {
                        allCorrect = false;
                        break;
                    }
                }
                if (allCorrect)
                {
                    miners[idx].blockchain = task.blockchain;
                    if (miners[idx].blk_crt_pending)
                    {
                        vector<Task> newTasks;
                        while (!miners[idx].tasks.empty())
                        {
                            Task curTask = miners[idx].tasks.top();
                            miners[idx].tasks.pop();
                            if (curTask.type == blk_crt)
                                continue;
                            newTasks.push_back(curTask);
                        }
                        for (auto it : newTasks)
                        {
                            miners[idx].tasks.push(it);
                        }
                        miners[idx].blk_crt_pending = false;
                    }
                }
            }
            break;
            case txn_crt:
            {
                TXN txn = createTransaction(miners[idx], txnId++, n_peers);
                if (txn.sender_bal >= txn.amount)
                {
                    miners[idx].knownTxns.insert(txn.txn_id);
                    miners[idx].validatedTxns.push_back(task.txn);

                    // Send it to all the peers
                    for (int i = 0; i < neighbours.size(); i++)
                    {
                        Task rcvTask = prepareTaskForTxnRcv(latency(miners[idx], miners[neighbours[i]], 't', prop_delay, gen) * 1000, txn);
                        miners[neighbours[i]].tasks.push(rcvTask);
                    }
                }
            }
            break;
            case txn_rcv:
            {
                if (task.txn.sender_bal >= task.txn.amount)
                {
                    miners[idx].validatedTxns.push_back(task.txn);
                    if (task.txn.receiver_id != miners[idx].peer_id)
                    {
                        for (int i = 0; i < neighbours.size(); i++)
                        {
                            auto knownTxns = miners[neighbours[i]].knownTxns;
                            if (knownTxns.find(task.txn.txn_id) == knownTxns.end())
                            {
                                Task rcvTask = prepareTaskForTxnRcv(latency(miners[idx], miners[neighbours[i]], 't', prop_delay, gen) * 1000, task.txn);
                                miners[neighbours[i]].tasks.push(rcvTask);
                            }
                        }
                    }
                    else
                    {
                        miners[idx].amnt += task.txn.amount;
                    }
                }
                miners[idx].knownTxns.insert(task.txn.txn_id);
            }
            break;
            }
            miners[idx].tasks.pop();
            miner_idx.pop();
        }
        global_time += smallest_time;

        map<int, Task> txnCrtTasks = prepareTasksForTxnCrt(n_peers); // Create random transaction generate events
        for (auto it : txnCrtTasks)
        {
            int idx = it.first;
            Task task = it.second;
            miners[idx].tasks.push(task);
        }
    }
}

int main()
{
    int n_peers;
    long time_limit;
    int lambda;

    cout << "\nEnter the number of peers in the network: ";
    cin >> n_peers;
    cout << "\nMean time of interarrival in seconds: ";
    cin >> lambda;
    cout << "\nDuration for the simulation to run in seconds: ";
    cin >> time_limit;

    vector<Node> miners = initialization(n_peers, global_time);

    static mt19937 gen(rand());

    // Run the simulation
    manager(miners, time_limit, lambda, gen);

    cout << "\nSimulation ended at time " << global_time << " seconds";

    cout << "\nPrinting network topology\n";
    printGraph(miners);

    return dumpSimulationResults(miners);
}
