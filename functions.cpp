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

// Function to generate a random number from an exponential distribution
double generateExponential(double mean)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dist(1.0 / mean);
    return dist(gen);
}

// latency .cpp
double latency(Node sender, Node receiver, char event, double prop_delay, mt19937 &gen)
{
    double m;

    if (event == 't')
    {
        m = 8 * 1024;
    }
    else if (event == 'b')
    {
        m = sender.blockchain[0].txn_tree.size() * 8 * 1024;
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
int generateRandom(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

// Function to create a random network topology using adjacency matrix
vector<vector<int > > createRandomTopology(int numPeers) {
    mt19937 gen(time(0));
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

vector<Node> initialization(long numPeers, long global_time)
{
    
    // Create an initial random network 
    vector<vector<int > > list_connections = createRandomTopology(numPeers);
    
    // Check if the graph is connected, recreate the graph until it is connected
    while (!isConnected(list_connections, numPeers)) {
        cout << "Generated graph is not connected. Recreating..." << endl;
        list_connections = createRandomTopology(numPeers);
    }

    vector<Node> peers;
    float z_slow;
    float z_lowcpu;

    cout << "\nEnter the percentage of slow peers in the network (%): ";
    cin >> z_slow;
    cout << "\nEnter the percentage of low cpu in the network (%): ";
    cin >> z_lowcpu;

    float z_fastcpu = 1 - z_lowcpu;
    float z_fast = 1 - z_slow;

    // Creation of the genesis block
    Block genesis;
    genesis.blk_id = 0;
    genesis.crt_time = global_time;
    genesis.txn_tree = vector<TXN>();  // Assuming TXN is the type of elements in txn_tree


    for (int i = 0; i < numPeers; i++)
    {
        Node peer;
        peer.peer_id = i;
        peer.cpu = (i < z_slow * numPeers) ? peer.cpu = 0 : 1;
        peer.speed =  (i < z_lowcpu * numPeers) ? 0 : 1;
        peer.blockchain = vector<Block>(1, genesis);
        peer.peer_nbh = vector<long>();  // Assuming long is the type of elements in peer_nbh
        cout << peer.peer_id << " is connected to: ";
        for (int j = 0; j < list_connections[i].size(); j++)
        {
            peer.peer_nbh.push_back(list_connections[i][j]);
            cout << list_connections[i][j] << " ";
        }
        cout << endl;
        peers.push_back(peer);
    }
    
    return peers;
}

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers)
{
    for (int i = 0; i < n_peers; i++)
    {
        if (p[i].peer_id == peer_id)
        {
            p[i].amnt += amount;
        }
    }
};

int getBalance(vector<Node> p, int peer_id, int n_peers)
{
    for (int i = 0; i < n_peers; i++)
    {
        if (p[i].peer_id == peer_id)
        {
            cout << "Peer " << peer_id << " has " << p[i].amnt << " coins" << endl;
            return p[i].amnt;
        }
    }
    return 0;
};

Block prepareNewBlock(int id, int crt_time){
    Block block;
    block.blk_id = id;
    block.crt_time = crt_time;
    return block;
}

// task . cpp

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

// transactions . cpp

// Function to create transactions between random peers with exponential interarrival times
TXN createTransaction(Node miner, int id, long n_peers)
{
    TXN txn;

    srand(static_cast<unsigned>(time(0)));           // Seed for random number generation
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

TXN createCoinbaseTransaction(int id, int txnId)
{
    TXN txn;
    txn.coinbase = true;
    txn.sender_id = -1; // Sender is not there for coinbase
    txn.receiver_id = id;
    txn.amount = 50;
    txn.txn_id = txnId;
    return txn;
}

bool verifyTransactions(Block block)
{
    vector<TXN> txns = block.txn_tree;
    for (int i = 0; i < txns.size(); i++)
    {
        if (txns[i].amount > txns[i].sender_bal)
            return false;
    }
    return true;
}

map<int, Task> prepareTasksForTxnCrt(long n_peers)
{
    map<int, Task> tasks;
    set<int> ids;
    int taskCount = 0;
    while (taskCount < 10) // Generating random 10 transactions after every iteration
    {
        int senderId = generateRandom(0, n_peers);
        while (ids.find(senderId) != ids.end())
            senderId = generateRandom(0, n_peers);
        Task task = prepareTaskForTxnCrt(5);
        tasks[senderId] = task;
        taskCount++;
    }
    return tasks;
}