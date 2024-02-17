#include <iostream>
#include <vector>
#include "node.hpp"
#include "task.hpp"
#include <stdbool.h>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

// Function to generate a random integer within a given range
int generateRandom(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> distribution(min, max);
    return distribution(gen);
}

// Function to check connectedness of the graph using DFS
bool isConnected(const vector<vector<int>>& connections, int numNodes) {
    vector<bool> visited(numNodes, false);

    function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        for (int neighbor : connections[node]) {
            if (!visited[neighbor]) {
                dfs(neighbor);
            }
        }
    };
    // Check connectivity for each node individually
    for (int node = 0; node < numNodes; ++node) {
        if (!visited[node]) {
            dfs(node);
        }
    }
    // Check if all nodes are visited
    return all_of(visited.begin(), visited.end(), [](bool v) { return v; });
}

// Function to create a random network topology using adjacency matrix
vector<vector<int>> createRandomTopology(int numPeers) {
    mt19937 gen(time(0));
    vector<vector<int>> connections(numPeers);

    for (int i = 0; i < numPeers; ++i) {
        int numConnections = gen() % 4 + 3;

        vector<int> possibleConnections(numPeers);
        iota(possibleConnections.begin(), possibleConnections.end(), 0);
        possibleConnections.erase(remove(possibleConnections.begin(), possibleConnections.end(), i), possibleConnections.end());

        shuffle(possibleConnections.begin(), possibleConnections.end(), gen);

        connections[i].insert(connections[i].end(), possibleConnections.begin(), possibleConnections.begin() + numConnections);
    }

    return connections;
}

vector<Node> initialization(long numPeers, long global_time)
{
    
    // Create an initial random network 
    vector<vector<int>> list_connections = createRandomTopology(numPeers);
    
    // Check if the graph is connected, recreate the graph until it is connected
    while (!isConnected(list_connections, numPeers)) {
        cout << "Generated graph is not connected. Recreating..." << endl;
        list_connections = createRandomTopology(numPeers);
    }

    vector<Node> peers;
    float z_slow;
    float z_lowcpu;

    cout << "\nEnter the percent of slow peer in the network: ";
    cin >> z_slow;
    cout << "\nEnter the percent of low cpu in the network: ";
    cin >> z_lowcpu;

    float z_fastcpu = 1 - z_lowcpu;
    float z_fast = 1 - z_slow;

    // Creation of the genesis block
    Block genesis;
    genesis.blk_id = 0;
    genesis.crt_time = global_time;
    genesis.txn_tree = {};

    for (int i = 0; i < numPeers; i++)
    {
        Node peer;
        peer.peer_id = i;
        peer.cpu = (i < z_slow * numPeers) ? peer.cpu = 0 : 1;
        peer.speed =  (i < z_lowcpu * numPeers) ? 0 : 1;
        peer.blockchain = {genesis}; // adding genesis block
        peer.peer_nbh = {};
        cout << peer.peer_id << " is connected to: ";
        for (int j = 0; j < peer.peer_nbh.size(); j++)
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
