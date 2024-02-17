#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

using namespace std;

vector<int> getPeerConnections(int peerId, const vector<vector<int>>& adjacencyMatrix) {
    vector<int> connections;
    for (int i = 0; i < adjacencyMatrix[peerId].size(); ++i) {
        if (adjacencyMatrix[peerId][i] == 1) {
            connections.push_back(i);
        }
    }
    return connections;
}

vector<vector<int>> createRandomTopology(int numPeers) {
    // Create an mt19937 random number engine
    mt19937 gen(time(0));

    // Create an adjacency matrix to represent the peer network
    vector<vector<int>> adjacencyMatrix(numPeers, vector<int>(numPeers, 0));

    // Iterate over each peer to establish connections
    for (int i = 0; i < numPeers; ++i) {
        // Generate a random number of connections (3 to 6)
        int numConnections = gen() % 4 + 3;

        // Generate a list of all possible connections
        vector<int> possibleConnections(numPeers);
        iota(possibleConnections.begin(), possibleConnections.end(), 0);
        possibleConnections.erase(remove(possibleConnections.begin(), possibleConnections.end(), i), possibleConnections.end());

        // Randomly shuffle the possible connections
        shuffle(possibleConnections.begin(), possibleConnections.end(), gen);

        // Select the first 'numConnections' connections and add them to the adjacency matrix
        for (int j = 0; j < numConnections; ++j) {
            int neighbor = possibleConnections[j];
            adjacencyMatrix[i][neighbor] = 1;
            adjacencyMatrix[neighbor][i] = 1;  // Ensure symmetry for undirected graph
        }
    }

    return adjacencyMatrix;
}

int main() {
    // Example usage:
    int numPeers = 10;
    vector<vector<int>> adjacencyMatrix = createRandomTopology(numPeers);

    int peerIdToCheck = 3;  // Replace with the desired peer ID
    vector<int> connections = getPeerConnections(peerIdToCheck, adjacencyMatrix);

    cout << "Peer " << peerIdToCheck << " is connected to: ";
    for (int connection : connections) {
        cout << connection << " ";
    }
    cout << endl;

    return 0;
}
