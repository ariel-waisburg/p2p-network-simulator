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
#define Pll pair<long, long>
using namespace std;

// Global variable to track current time of simulation in milliseconds
long global_time = 0;

// Function declaration for manager logic
void manager(long time_limit, vector<Node> miners) {}

int main()
{
    // Variables to store user input
    long n_peers; // Number of peers in the network
    long time_limit; // Duration for the simulation to run in seconds
    long lambda; // Mean time of interarrival in seconds

    // User input for simulation parameters
    cout << "\nEnter the number of peers in the network: ";
    cin >> n_peers;
    cout << "\nMean time of interarrival in seconds: ";
    cin >> lambda;
    cout << "\nDuration for the simulation to run in seconds: ";
    cin >> time_limit;

    // Initialize miners and simulation parameters
    vector<Node> miners = initialization(n_peers, global_time); // Initialize network nodes
    set<long> txnSet; // Global set to track used transactions as UTXOs
    long blkId = 1; // Unique ID for blocks created in increasing format
    long txnId = 1; // Unique ID for transactions created in increasing format

    // Random number generator setup
    static mt19937 gen(rand()); // Mersenne Twister pseudo-random number engine
    uniform_real_distribution<double> distribution(0.01, 0.5); // Uniform distribution for propagation delay

    // Generate a random propagation delay
    double prop_delay = distribution(gen);

    // Simulation loop
    while (global_time < time_limit)
    {
        // Priority queue to track tasks of miners
        priority_queue<Pll, vector<Pll>, greater<Pll > > miner_idx;
        
        // Loop through each peer and schedule tasks
        for (long i = 0; i < n_peers; i++)
        {
            // Schedule block creation task if not pending
            if (!miners[i].blk_crt_pending)
            {
                miners[i].tasks.push(prepareTaskForBlockCreate(generateExponential(lambda) * 1000));
                miners[i].blk_crt_pending = true;
            }
            miner_idx.push(make_pair(miners[i].tasks.top().trigger_time, i)); // Push task into priority queue
        }

        // Get the smallest time from priority queue
        long smallest_time = miner_idx.top().first;

        // Process tasks with smallest time
        while (!miner_idx.empty())
        {
            long current_task_time = miner_idx.top().first;
            if (current_task_time > smallest_time)
                break;

            long idx = miner_idx.top().second;
            Task task = miners[idx].tasks.top();
            vector<long> neighbours = miners[idx].peer_nbh;
            vector<TXN> txns = miners[idx].validatedTxns;

            /* Do whatever operation you have to do with task.
            Just make sure to invert blk_crt_pending to false if
            the current task was to create a block itself.

            Also if any events like sending a block to some other
            nodes happnes then introduce task with correct time in
            that other miners task list */

            // Handle different task types
            switch (task.type)
            {
            case blk_crt:
            {
                // Prepare new block and coinbase transaction
                Block newBlock = prepareNewBlock(blkId++, global_time + smallest_time);
                TXN reward = createCoinbaseTransaction(miners[idx].peer_id, txnId++);
                newBlock.txn_tree.push_back(reward);
                txnSet.insert(reward.txn_id);

                // Select valid transactions for the block
                for (long i = 0; i < txns.size(); i++)
                {
                    if (txnSet.find(txns[i].txn_id) == txnSet.end())
                    {
                        txnSet.insert(txns[i].txn_id);
                        newBlock.txn_tree.push_back(txns[i]);
                        if (newBlock.txn_tree.size() == 1024)
                        { // Break after 1MB
                            miners[idx].validatedTxns.erase(miners[idx].validatedTxns.begin());
                            break;
                        }
                    }
                    miners[idx].validatedTxns.erase(miners[idx].validatedTxns.begin());
                }

                // Add new block to miner's blockchain and propagate to neighbors
                miners[idx].blockchain.push_back(newBlock);
                for (long i = 0; i < neighbours.size(); i++)
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
                // Perform proof-of-work validation
                long j = 0;
                for (long i = 0; i < miners[idx].blockchain.size(); i++)
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
                for (long i = j; i < task.blockchain.size(); i++)
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
                // Create transaction and validate sender balance
                TXN txn = createTransaction(miners[idx], txnId++, n_peers);
                if (txn.sender_bal >= txn.amount)
                {
                    miners[idx].knownTxns.insert(txn.txn_id);
                    miners[idx].validatedTxns.push_back(task.txn);

                    // Send it to all the peers
                    for (long i = 0; i < neighbours.size(); i++)
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
                        // Send transaction to all peers
                        for (long i = 0; i < neighbours.size(); i++)
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

    cout << "\nSimulation ended at time " << global_time << " seconds";

    cout << "\nPrinting network topology\n";
    printGraph(miners);

    // Open a file for writing
    ofstream outputFile("simulation_output.txt");

    // Check if the file is opened successfully
    if (!outputFile.is_open())
    {
        cout << "Error opening file for writing!" << endl;
        return 1; // Return error code
    }

    // Traverse through each node in miners vector
    for (const auto &node : miners)
    {
        // Write node ID and blockchain list
        outputFile << "Node ID: " << node.peer_id << endl;
        outputFile << "Blockchain:" << endl;
        for (const auto &block : node.blockchain)
        {
            outputFile << "Block ID: " << block.blk_id << ", Transactions: ";
            for (const auto &txn : block.txn_tree)
            {
                outputFile << "{ID: " << txn.txn_id << ", Sender: " << txn.sender_id
                           << ", Receiver: " << txn.receiver_id << ", Amount: " << txn.amount << "} ";
            }
            outputFile << endl;
        }

        // Write peer relationships
        outputFile << "Peer Relationships:" << endl;
        for (const auto &neighbor : node.peer_nbh)
        {
            outputFile << "Connected to Node ID: " << neighbor << endl;
        }

        outputFile << endl; // Add a blank line between nodes
    }

    // Close the file
    outputFile.close();
    return 0;
}
