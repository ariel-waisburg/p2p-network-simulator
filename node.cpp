#include <iostream>
#include <vector>
#include "node.hpp"
#include <stdbool.h>

using namespace std;

class Node {
    public:
        int peer_id;
        int cpu; // 0 or 1
        int speed;
};

vector<int> initialization(int n_peers) {

    float z_slow;
    float z_lowcpu;

    cout << "Enter the percent of slow peer in the network: ";
    cin >> z_slow; 
    cout << "Enter the percent of low cpu in the network: ";
    cin >> z_lowcpu; 

    float z_fastcpu = 1 - z_lowcpu;
    float z_fast = 1 - z_slow;

    for (int i = 10000; i+10000 < n_peers+10000; i++) {
        
        Node peer;
        peer.peer_id = i;
        peer.cpu = 0;
        peer.speed = 1;

        cout << "Peer " << peer.peer_id << " created" << endl; 

        vector<Node> peers;
        peers.push_back(peer);
    }
}

//Debugging
int main(){
    initialization();
    return 0;
}