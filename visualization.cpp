#include <iostream>
#include <fstream>
#include <vector>
#include "models.cpp"

int printGraph(vector<Node> miners)
{
    // Open a file to write the graph description in DOT language
    std::ofstream dotFile("graph.dot");
    set<pair<int, int>> edges;

    // Write the graph description in DOT language
    dotFile << "graph MyGraph {\n";
    for (int i = 0; i < miners.size(); i++)
    {
        for (int j = 0; j < miners[i].peer_nbh.size(); j++)
        {
            if (edges.find({miners[i].peer_nbh[j], i}) == edges.end())
            {
                dotFile << "    " << i << " -- " << miners[i].peer_nbh[j] << ";\n";
                edges.insert({i, miners[i].peer_nbh[j]});
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
