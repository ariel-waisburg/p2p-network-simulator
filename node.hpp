#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <queue>
#include "task.hpp"

using namespace std;

class Node
{
public:
    int peer_id; // 1 to ...
    int cpu;     // 0 or 1
    int speed;   // 0 or 1
    int amnt;
    bool blk_crt_pending = true;
    vector<int> peer_nbh;
    priority_queue<Task, vector<Task>, Compare> tasks;
};

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers);

int getBalance(vector<Node> p, int peer_id, int n_peers);

vector<Node> initialization();

#endif
