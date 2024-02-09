#ifndef NODE_HPP
#define NODE_HPP

#include <vector>

using namespace std;

class Node
{
public:
    int peer_id; // 1 to ...
    int cpu;     // 0 or 1
    int speed;   // 0 or 1
    int amnt;
    vector<Node> peer_nbh;
};

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers);

int getBalance(vector<Node> p, int peer_id, int n_peers);

vector<int> initialization(void);

#endif
