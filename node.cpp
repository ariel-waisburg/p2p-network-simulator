#include <iostream>
#include <vector>
#include "node.hpp"
#include <stdbool.h>

using namespace std;

void updateBalance(vector<Node> p, int peer_id, int amount, int n_peers)
{
    for (int i = 0; i < n_peers; i++)
    {
        if (p[i].peer_id == peer_id)
        {
            p[i].amnt += amount;
        }
    }
};

int getBalance(vector<Node> p, int peer_id, int n_peers)
{
    for (int i = 0; i < n_peers; i++)
    {
        if (p[i].peer_id == peer_id)
        {
            cout << "Peer " << peer_id << " has " << p[i].amnt << " coins" << endl;
            return p[i].amnt;
        }
    }
    return 0;
};

vector<int> initialization(int n_peers)
{

    float z_slow;
    float z_lowcpu;

    cout << "Enter the percent of slow peer in the network: ";
    cin >> z_slow;
    cout << "Enter the percent of low cpu in the network: ";
    cin >> z_lowcpu;

    float z_fastcpu = 1 - z_lowcpu;
    float z_fast = 1 - z_slow;

    for (int i = 10000; i + 10000 < n_peers + 10000; i++)
    {

        Node peer;
        peer.peer_id = i;
        peer.cpu = 0;
        peer.speed = 1;

        cout << "Peer " << peer.peer_id << " created" << endl;

        vector<Node> peers;
        peers.push_back(peer);
    }
}
