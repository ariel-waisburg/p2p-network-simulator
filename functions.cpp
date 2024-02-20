#include "models.cpp"
#include "utils.cpp"
#include <random>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <time.h>
#include <vector>
#include <stdbool.h>
#include <set>

using namespace std;
int seed = 0;
// Function to generate a random number from an exponential distribution
double generateExponential(double mean)
{
    mt19937 gen(time(0) + seed++);
    exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

// latency .cpp

double generatePropDelay()
{
    mt19937 gen(time(0) + seed++);
    uniform_real_distribution<double> distribution(0.01, 0.5);
    return distribution(gen);
}

double latency(Node sender, Node receiver, char event, double prop_delay)
{
    double m;

    if (event == 't')
    {
        m = 8 * 1024; // 1 KB = 1024 bytes = 1024 * 8 bits
    }
    else if (event == 'b')
    {
        int sz = sender.blockchain.size();
        m = sender.blockchain[sz - 1].txn_tree.size() * 8 * 1024; // Access quantity of transactions to determine message size
    }
    else
    {
        throw invalid_argument("event must be t or b");
    }
    double c;
    if (sender.speed && receiver.speed)
    {
        c = 100 * 1000000;
    }
    else
    {
        c = 5 * 1000000;
    }
    double d_mean = 96 * 1000 / c;
    double d = generateExponential(d_mean);
    double l_ij = prop_delay + m / c + d;

    return l_ij;
}

// Node cpp

// Function to generate a random integer within a given range
int generateRandom(int min, int max)
{
    mt19937 gen(time(0) + seed++);
    uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

// Function to create a random network topology using adjacency matrix
vector<vector<int > > createRandomTopology(int numPeers)
{
    mt19937 gen(time(0) + seed++);
    vector<vector<int > > connections(numPeers);

    vector<int> indexes(numPeers);
    iota(indexes.begin(), indexes.end(), 0);
    shuffle(indexes.begin(), indexes.end(), gen);

    for (int i = 0; i < numPeers; i++)
    {
        connections[indexes[i]].push_back(indexes[(i + 1) % numPeers]);
        connections[indexes[i]].push_back(indexes[(i - 1 + numPeers) % numPeers]);
    }
    for (int i = 0; i < numPeers / 2; i++)
    {
        connections[indexes[i]].push_back(indexes[(i + numPeers / 2)]);
        connections[indexes[(i + numPeers / 2)]].push_back(indexes[i]);
    }
    if (numPeers % 2 == 1)
    {
        connections[indexes[numPeers - 1]].push_back(indexes[numPeers / 2]);
        connections[indexes[numPeers / 2]].push_back(indexes[numPeers - 1]);
    }
    return connections;
}

vector<Node> initialization(int numPeers, int global_time)
{

    // Create an initial random network
    vector<vector<int > > list_connections = createRandomTopology(numPeers);

    // Check if the graph is connected, recreate the graph until it is connected
    while (!isConnected(list_connections, numPeers))
    {
        cout << "Generated graph is not connected. Recreating..." << endl;
        list_connections = createRandomTopology(numPeers);
    }

    vector<Node> peers;
    int z_slow;
    int z_lowcpu;

    cout << "\nEnter the percentage of slow peers in the network (%): ";
    cin >> z_slow;
    cout << "\nEnter the percentage of low cpu in the network (%): ";
    cin >> z_lowcpu;

    z_slow = (z_slow * numPeers) / 100;
    z_lowcpu = (z_lowcpu * numPeers) / 100;

    double lowHashPower = (double)1 / (double)(10 * numPeers - 9 * z_lowcpu);

    // Creation of the genesis block
    Block genesis;
    genesis.blk_id = 0;
    genesis.crt_time = global_time;
    genesis.txn_tree = vector<Txn>(); // Assuming Txn is the type of elements in txn_tree

    for (int i = 0; i < numPeers; i++)
    {
        Node peer;
        peer.peer_id = i;
        peer.hashPower = lowHashPower;
        if (generateRandom(0, 1) && z_slow > 0)
        {
            peer.cpu = 1;
            z_slow--;
        }
        if (generateRandom(0, 1) && z_lowcpu > 0)
        {
            peer.speed = 1;
            peer.hashPower = lowHashPower * 10;
            z_lowcpu--;
        }
        peer.blockchain = vector<Block>(1, genesis);
        peer.peer_nbh = vector<int>();
        cout << peer.peer_id << " is connected to: ";
        for (int j = 0; j < list_connections[i].size(); j++)
        {
            peer.peer_nbh.push_back(list_connections[i][j]);
            cout << list_connections[i][j] << " ";
        }
        cout << endl;
        cout << "Hash power: " << peer.hashPower << endl;
        peers.push_back(peer);
    }

    int idx = 0;
    while ((z_slow || z_lowcpu) && idx < numPeers)
    {
        if (!peers[idx].cpu && z_lowcpu > 0)
        {
            peers[idx].cpu = 1;
            peers[idx].hashPower = lowHashPower * 10;
            z_lowcpu--;
        }
        if (!peers[idx].speed && z_slow > 0)
        {
            peers[idx].speed;
            z_slow--;
        }
        idx++;
    }

    return peers;
}

Block prepareNewBlock(int id, int crt_time)
{
    Block block;
    block.blk_id = id;
    block.crt_time = crt_time;
    return block;
}

// task . cpp

Task prepareTaskForBlockCreate(int time)
{
    Task task;
    task.type = blk_crt;
    task.trigger_time = time;
    return task;
}

Task prepareTaskForBlockRecieve(int time, vector<Block> blockchain)
{
    Task task;
    task.type = blk_rcv;
    task.trigger_time = time;
    task.blockchain = blockchain;
    return task;
}

Task prepareTaskForTxnRcv(int time, Txn txn)
{
    Task task;
    task.type = txn_rcv;
    task.trigger_time = time;
    task.txn = txn;
    return task;
}

Task prepareTaskForTxnCrt(int time)
{
    Task task;
    task.type = txn_crt;
    task.trigger_time = time;
    return task;
}

// transactions . cpp

// Function to create transactions between random peers with exponential interarrival times
Txn createTransaction(Node miner, int id, int n_peers)
{
    Txn txn;

    srand(static_cast<unsigned>(time(0)));        // Seed for random number generation
    int receiver_id = generateRandom(0, n_peers); // Select a random receiver ID
    while (receiver_id == miner.peer_id)
        receiver_id = generateRandom(0, n_peers);

    txn.amount = generateRandom(1, 20); // Create a random amount between 1 to 20
    txn.sender_id = miner.peer_id;
    txn.sender_bal = miner.amnt;
    txn.receiver_id = receiver_id;
    txn.txn_id = id;

    return txn;
}

Txn createCoinbaseTransaction(int id, int txnId)
{
    Txn txn;
    txn.coinbase = true;
    txn.sender_id = -1; // Sender is not there for coinbase
    txn.receiver_id = id;
    txn.amount = 50;
    txn.txn_id = txnId;
    return txn;
}

vector<Txn> provideValidTransactions(Node miner)
{
    set<int> txnId;
    vector<Txn> txns;
    for (auto b : miner.blockchain)
    {
        for (auto t : b.txn_tree)
        {
            txnId.insert(t.txn_id);
        }
    }
    queue<Txn> q = miner.validatedTxns;
    while (!q.empty() && txns.size() < 1023) // break after 1MB including reward
    {
        auto txn = q.front();
        q.pop();
        if (txnId.find(txn.txn_id) == txnId.end())
        {
            txns.push_back(txn);
        }
    }
    return txns;
}

bool verifyTransactions(Block block)
{
    vector<Txn> txns = block.txn_tree;
    for (int i = 0; i < txns.size(); i++)
    {
        if (txns[i].amount > txns[i].sender_bal)
            return false;
    }
    return true;
}

map<int, Task> prepareTasksForTxnCrt(int n_peers)
{
    map<int, Task> tasks;
    set<int> ids;
    int taskCount = 0;
    while (taskCount < 3) // Generating random transactions create event in approx txn_interarrival seconds
    {
        int senderId = generateRandom(0, n_peers);
        while (ids.find(senderId) != ids.end())
            senderId = generateRandom(0, n_peers);
        ids.insert(senderId);
        double delay = generateExponential(0.5); // Generate a delay of less than a second around current time
        cout << "Delay for txn_crt: " << delay << endl;
        Task task = prepareTaskForTxnCrt(delay * 1000);
        tasks[senderId] = task;
        taskCount++;
    }
    return tasks;
}