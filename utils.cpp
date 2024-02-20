#include <algorithm>
#include <numeric>
#include <functional>
#include <vector>
using namespace std;

// Depth-first search to traverse the graph and mark visited nodes
void dfs(int node, vector<bool> &visited, const vector<vector<int > > &connections)
{
    visited[node] = true;
    for (int neighbor : connections[node])
    {
        if (!visited[neighbor])
        {
            dfs(neighbor, visited, connections);
        }
    }
};

// Function to check if a node is visited
bool isNodeVisited(bool v) {
    return v;
}

// Function to check connectedness of the graph using DFS
bool isConnected(const vector<vector<int > > &connections, int numNodes)
{
    vector<bool> visited(numNodes, false);

    // Check connectivity for each node individually
    for (int node = 0; node < numNodes; ++node)
    {
        if (!visited[node])
        {
            dfs(node, visited, connections);
        }
    }
    // Check if all nodes are visited
    return all_of(visited.begin(), visited.end(), isNodeVisited);
}
