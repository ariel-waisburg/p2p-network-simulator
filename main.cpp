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
#define P pair<int, int>
using namespace std;

// Global variable to track current time of simulation in milliseconds
int global_time = 0;
int last_txn_generation = 0;

// Logic for managing the simulation
void manager(vector<Node> &miners, int time_limit, int txn_interval, double lambda)
{
    // Variables to store user input
    int n_peers = miners.size();
    int blkId = 1;                           // Unique Id for blocks created in increasing format
    int txnId = 1;                           // Unique Id for transactions created in increasing format
    double prop_delay = generatePropDelay(); // Generate a random propagation delay
    char ch;

    // Simulation loop
    while (global_time < time_limit)
    {
        // Priority queue to track tasks of miners
        priority_queue<P, vector<P>, greater<P > > pq;

        // Loop through each peer and schedule tasks
        for (int i = 0; i < n_peers; i++)
        {
            // Schedule block creation task if not pending
            if (!miners[i].blk_crt_pending)
            {
                double delay = generateExponential(lambda / miners[i].hashPower);
                cout << "Delay for blk_crt: " << delay << endl;
                miners[i].tasks.push(prepareTaskForBlockCreate(delay * 1000));
                miners[i].blk_crt_pending = true;
            }
            pq.push(make_pair(miners[i].tasks.top().trigger_time, i));
        }

        // Get the smallest time from priority queue
        int smallest_time = pq.top().first;

        while (!pq.empty())
        {
            auto m = pq.top();
            pq.pop();
            int current_task_time = m.first;
            if (current_task_time > smallest_time)
                break;

            int idx = m.second;
            Task task = miners[idx].tasks.top();
            miners[idx].tasks.pop();
            vector<int> neighbours = miners[idx].peer_nbh;
            queue<Txn> txns = miners[idx].validatedTxns;

            // Do whatever operation you have to do with task.
            // Just make sure to invert blk_crt_pending to false if
            // the current task was to create a block itself.

            // Also if any events like sending a block to some other
            // nodes happnes then introduce task with correct time in
            // that other miners task list

            // Handle different task types
            switch (task.type)
            {
            case blk_rcv:
            {
                if (task.blockchain.size() > miners[idx].blockchain.size())
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

                    // Perform proof-of-work validation
                    for (int i = j; i < task.blockchain.size(); i++)
                    {
                        // Check if the receieved blockchain has valid transactions after fork
                        if (!verifyTransactions(task.blockchain[i]))
                        {
                            allCorrect = false;
                            break;
                        }
                    }
                    if (allCorrect) // Delete current miner's block creation task
                    {
                        miners[idx].blockchain.clear();
                        miners[idx].blockchain = task.blockchain;
                        if (miners[idx].blk_crt_pending)
                        {
                            vector<Task> newTasks;
                            while (!miners[idx].tasks.empty())
                            {
                                Task curTask = miners[idx].tasks.top();
                                miners[idx].tasks.pop();
                                if (curTask.type != blk_crt)
                                    newTasks.push_back(curTask);
                            }
                            for (auto it : newTasks)
                            {
                                miners[idx].tasks.push(it);
                            }
                            miners[idx].blk_crt_pending = false;
                        }
                        // Broadcast block to neighbours
                        for (int i = 0; i < neighbours.size(); i++)
                        {
                            double delay = latency(miners[idx], miners[neighbours[i]], 'b', prop_delay);
                            cout << "Delay for blk_rcv: " << delay << endl;
                            Task rcvTask = prepareTaskForBlockRecieve(delay * 1000, miners[idx].blockchain);
                            miners[neighbours[i]].tasks.push(rcvTask);
                        }
                    }
                }
            }
            break;
            case blk_crt:
            {
                // Prepare new block and coinbase transaction
                Block newBlock = prepareNewBlock(blkId++, global_time + smallest_time);
                Txn reward = createCoinbaseTransaction(miners[idx].peer_id, txnId++);
                newBlock.txn_tree.push_back(reward);

                // Select valid transactions for the block
                vector<Txn> validTxns = provideValidTransactions(miners[idx]);
                for (auto txn : validTxns)
                {
                    newBlock.txn_tree.push_back(txn);
                }

                miners[idx].blockchain.push_back(newBlock);

                cout << "pow_done successful" << endl;

                // Add new block to miner's blockchain and propagate to neighbors
                for (int i = 0; i < neighbours.size(); i++)
                {
                    double delay = latency(miners[idx], miners[neighbours[i]], 'b', prop_delay);
                    cout << "Delay for blk_rcv: " << delay << endl;
                    Task rcvTask = prepareTaskForBlockRecieve(delay * 1000, miners[idx].blockchain);
                    miners[neighbours[i]].tasks.push(rcvTask);
                }
                miners[idx].amnt += 50; // Incremented by 50 coins for coinbase transaction
                miners[idx].blk_crt_pending = false;
            }
            break;
            case txn_crt:
            {
                // Create transaction and validate sender balance
                Txn txn = createTransaction(miners[idx], txnId++, n_peers);
                if (txn.sender_bal >= txn.amount)
                {
                    miners[idx].knownTxns.insert(txn.txn_id);
                    miners[idx].validatedTxns.push(task.txn);

                    // Send transaction to all peers
                    for (int i = 0; i < neighbours.size(); i++)
                    {
                        double delay = latency(miners[idx], miners[neighbours[i]], 't', prop_delay);
                        cout << "Delay for txn_rcv: " << delay << endl;
                        Task rcvTask = prepareTaskForTxnRcv(delay * 1000, txn);
                        miners[neighbours[i]].tasks.push(rcvTask);
                    }
                }
            }
            break;
            case txn_rcv:
            {
                // Validate sender balance
                if (task.txn.sender_bal >= task.txn.amount)
                {
                    miners[idx].validatedTxns.push(task.txn);
                    if (task.txn.receiver_id != miners[idx].peer_id)
                    {
                        // Send transaction to all peers
                        for (int i = 0; i < neighbours.size(); i++)
                        {
                            auto knownTxns = miners[neighbours[i]].knownTxns;
                            if (knownTxns.find(task.txn.txn_id) == knownTxns.end())
                            {
                                double delay = latency(miners[idx], miners[neighbours[i]], 't', prop_delay);
                                cout << "Delay for txn_rcv: " << delay << endl;
                                Task rcvTask = prepareTaskForTxnRcv(delay * 1000, task.txn);
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
        }

        global_time += smallest_time;
        cout << "Global time: " << global_time << " Smallest time: " << smallest_time << endl;

        if (global_time - last_txn_generation >= txn_interval) // Producing transactions
        {
            last_txn_generation = global_time;
            map<int, Task> txnCrtTasks = prepareTasksForTxnCrt(n_peers); // Create random transaction generate events
            for (auto it : txnCrtTasks)
            {
                int idx = it.first;
                Task task = it.second;
                miners[idx].tasks.push(task);
            }
        }
    }
}

int main()
{
    // Variables to store user input
    int n_peers;
    double time_limit;
    double lambda;
    double txn_arrival;

    // User input for simulation parameters
    cout << "\nEnter the number of peers in the network: ";
    cin >> n_peers;
    cout << "\nMean time of block interarrival in seconds: ";
    cin >> lambda;
    cout << "\nMean number of transaction per second: ";
    cin >> txn_arrival;
    cout << "\nDuration for the simulation to run in seconds: ";
    cin >> time_limit;

    // Initialize miners and simulation parameters
    vector<Node> miners = initialization(n_peers, global_time);

    // Run the simulation
    manager(miners, time_limit * 1000, ((double)1 / txn_arrival) * 1000, lambda);

    cout << "\nSimulation ended at time " << global_time << " milliseconds";

    cout << "\nPrinting network topology\n";
    printGraph(miners);

    return dumpSimulationResults(miners);
}
