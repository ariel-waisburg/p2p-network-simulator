#include <iostream>
#include <fstream>
#include <vector>
#include "models.cpp"

int printGraph(vector<Node> miners)
{
    // Open a file to write the graph description in DOT language
    std::ofstream dotFile("graph.dot");
    set<pair<int, int > > edges;

    // Write the graph description in DOT language
    dotFile << "graph MyGraph {\n";
    for (int i = 0; i < miners.size(); i++)
    {
        for (int j = 0; j < miners[i].peer_nbh.size(); j++)
        {
            if (edges.find(std::make_pair(miners[i].peer_nbh[j], i)) == edges.end())
            {
                dotFile << "    " << i << " -- " << miners[i].peer_nbh[j] << ";\n";
                edges.insert(make_pair(i, miners[i].peer_nbh[j]));
            }
        }
    }
    dotFile << "}\n";

    // Close the file
    dotFile.close();

    // Use Graphviz to generate the visual representation
    system("dot -Tpng graph.dot -o graph.png");

    std::cout << "Graph visualization generated as graph.png" << std::endl;

    return 0;
}

int dumpSimulationResults(vector<Node> &miners)
{
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

        outputFile << "Task List:" << endl;
        auto tasks = node.tasks;
        while (!tasks.empty())
        {
            auto t = tasks.top();
            tasks.pop();
            outputFile << "Task Type: " << t.type << " Trigger Time: " << t.trigger_time << " Txn: ";
            outputFile << "{ID: " << t.txn.txn_id << ", Sender: " << t.txn.sender_id
                       << ", Receiver: " << t.txn.receiver_id << ", Amount: " << t.txn.amount << "} " << endl;
            outputFile << "Blockchain size: " << t.blockchain.size() << endl;
        }

        outputFile << endl; // Add a blank line between nodes
    }

    // Close the file
    outputFile.close();
    return 0;
}

void printPeer(Node node)
{
    cout << "Node ID: " << node.peer_id << endl;
    cout << "Blockchain:" << endl;
    for (const auto &block : node.blockchain)
    {
        cout << "Block ID: " << block.blk_id << ", Transactions: ";
        for (const auto &txn : block.txn_tree)
        {
            cout << "{ID: " << txn.txn_id << ", Sender: " << txn.sender_id
                 << ", Receiver: " << txn.receiver_id << ", Amount: " << txn.amount << "} ";
        }
        cout << endl;
    }
    cout << "Task Size: " << node.tasks.size() << endl;
    auto task = node.tasks.top();
    cout << "Task Type: " << task.type << endl;
}
