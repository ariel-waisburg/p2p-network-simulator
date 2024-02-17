#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

// Function to create an adjacency list for a peer network
std::vector<std::vector<int>> createAdjacencyList(int n) {
    // Initialize random seed
    std::srand(std::time(0));

    // Create an adjacency list to represent the peer network
    std::vector<std::vector<int>> adjacencyList(n);

    // Iterate over each peer to establish connections
    for (int i = 0; i < n; ++i) {
        // Generate a random number of connections (3 to 6)
        int numConnections = rand() % 4 + 3;

        // Generate a list of all possible connections
        std::vector<int> possibleConnections(n);
        std::iota(possibleConnections.begin(), possibleConnections.end(), 0);
        possibleConnections.erase(std::remove(possibleConnections.begin(), possibleConnections.end(), i), possibleConnections.end());

        // Randomly shuffle the possible connections
        std::shuffle(possibleConnections.begin(), possibleConnections.end(), std::default_random_engine(std::rand()));

        // Select the first 'numConnections' connections and add them to the adjacency list
        for (int j = 0; j < numConnections; ++j) {
            adjacencyList[i].push_back(possibleConnections[j]);
        }
    }

    return adjacencyList;
}

int main() {
    // Specify the number of peers
    int numberOfPeers = 10;

    // Create the adjacency list for the peer network
    std::vector<std::vector<int>> adjacencyList = createAdjacencyList(numberOfPeers);

    // Display the adjacency list
    for (int i = 0; i < numberOfPeers; ++i) {
        std::cout << "Peer " << i << " is connected to: ";
        for (int j : adjacencyList[i]) {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
