#include <iostream>
#include <vector>
#include "node.hpp"
#include "task.hpp"
#include <stdbool.h>

using namespace std;

vector<Node> initialization(long n_peers)
{
    vector<Node> peers;
    float z_slow;
    float z_lowcpu;

    cout << "\nEnter the percent of slow peer in the network: ";
    cin >> z_slow;
    cout << "\nEnter the percent of low cpu in the network: ";
    cin >> z_lowcpu;

    float z_fastcpu = 1 - z_lowcpu;
    float z_fast = 1 - z_slow;

    for (int i = 0; i < n_peers; i++)
    {

        Node peer;
        peer.peer_id = i + 10000;
        peer.cpu = 0;
        peer.speed = 1;
        peer.tasks.push(prepareForBlockCreate(5)); // instead of default 5 there should be a random value

        cout << "Peer " << peer.peer_id << " created" << endl;
        peers.push_back(peer);
    }
    return peers;
}

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
