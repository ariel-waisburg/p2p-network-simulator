#include <iostream>
#include <vector>
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


int main() {
    int numPeers;

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

    cout << "Generated connected graph successfully." << endl;

    // Display the connections
    cout << "Connections:" << endl;
    for (int i = 0; i < numPeers; ++i) {
        cout << "Peer " << i << " is connected to: ";
        for (int j = 0; j < numPeers; ++j) {
            if (adjacencyMatrix[i][j] == 1) {
                cout << j << " ";
            }
        }
        cout << endl;
    }
    
    return 0;
}
