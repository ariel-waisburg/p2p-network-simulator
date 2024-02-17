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
bool isConnected(const vector<vector<int>>& adjacencyMatrix, int numNodes) {
    vector<bool> visited(numNodes, false);

    function<void(int)> dfs = [&](int node) {
        visited[node] = true;
        for (int neighbor = 0; neighbor < numNodes; ++neighbor) {
            if (adjacencyMatrix[node][neighbor] && !visited[neighbor]) {
                dfs(neighbor);
            }
        }
    };
    
    dfs(0);

    return all_of(visited.begin(), visited.end(), [](bool v) { return v; });
}

// Function to create a random network topology using adjacency matrix
vector<vector<int>> createRandomTopology(int numPeers, mt19937& gen) {
    vector<vector<int>> adjacencyMatrix(numPeers, vector<int>(numPeers, 0));
    
    // Create edges between peers
    for (int i = 0; i < numPeers; ++i) {
        int numEdges = generateRandom(3, 6);

        set<int> chosenNeighbors;
        int maxAttempts = 10 * numEdges; // Set a maximum number of attempts

        while (chosenNeighbors.size() < numEdges && maxAttempts > 0) {
            int neighbor = generateRandom(0, numPeers - 1);
            if (neighbor != i) {
                chosenNeighbors.insert(neighbor);
            }
            maxAttempts--;
        }
        
        for (int neighbor : chosenNeighbors) { // Set connections   
            adjacencyMatrix[i][neighbor] = 1;  
            adjacencyMatrix[neighbor][i] = 1;  

        }
    }
    return adjacencyMatrix;
}

vector<Node> initialization()
{
    long numPeers;

    cout << "Enter the number of peers: ";
    cin >> numPeers;
    
    mt19937 gen(time(0));

    // Create an initial random network 
    vector<vector<int>> adjacencyMatrix = createRandomTopology(numPeers, gen);
    
    // Check if the graph is connected, recreate the graph until it is connected
    while (!isConnected(adjacencyMatrix, numPeers)) {
        cout << "Generated graph is not connected. Recreating..." << endl;
        adjacencyMatrix = createRandomTopology(numPeers, gen);
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

    for (int i = 0; i < numPeers; i++)
    {
        Node peer;
        peer.peer_id = i;
        peer.cpu = (i < z_slow * numPeers) ? 0 : 1;
        peer.speed =  (i < z_lowcpu * numPeers) ? 0 : 1;
        peer.tasks.push(prepareForBlockCreate(5)); // instead of default 5 there should be a random value
        adjacencyMatrix[i][i] = 0; // No self loops
        peer.peer_nbh = {};
        for (int j = 0; j<numPeers; j++)
        {
            if (adjacencyMatrix[peer.peer_id][j] == 1)
            {
                peer.peer_nbh.push_back(j);
            }
        }
        cout << peer.peer_id << " is connected to: ";
        for (int j = 0; j < peer.peer_nbh.size(); j++)
        {
            cout << peer.peer_nbh[j] << endl;
        }
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



int main() {
    initialization();
    //Using Peer id as 0 to n_peers so we can navigate in the adjacency list easily
    return 0;
}