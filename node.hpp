#ifndef NODE_HPP
#define NODE_HPP

#include <vector>

using namespace std;

class Node {
    public:
        int peer_id; //1 to ...
        int cpu; // 0 or 1
        int speed; // 0 or 1
        int amnt; 
        vector<Node> peer_nbh;
};

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers) {
    for (int i = 0; i < n_peers; i++){
        if (p[i].peer_id == peer_id){
            p[i].amnt += amount;
        }
    }
};

void getBalance(vector<Node> p, int peer_id, int n_peers) {
    for (int i = 0; i < n_peers; i++){
        if (p[i].peer_id == peer_id){
            cout << "Peer " << peer_id << " has " << p[i].amnt << " coins" << endl;
        }
    }
};

vector<int> initialization(void);

#endif
