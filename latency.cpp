#include <iostream>
#include <vector>
#include "node.hpp"
// #include "block.hpp" // to be checked with the inclusion of the block class
#include <random>

using namespace std;

// the prop_delay should be chosen at the start of the simulation, like this:
//double prop_delay = std::uniform_real_distribution<double>(0.01, 0.5)(std::default_random_engine()); // time in seconds, change to ms if needed

double latency(Node sender, Node receiver, char event, double prop_delay, mt19937& gen) {
    double m;
    Block b;
    if (event == 't') {
        m = 8 * 1000;
    } else if (event == 'b') {
        m = b.txn_tree.size() * 8 * 1000; // assuming in the block class we can access quantity of transactions
    } else {
        throw invalid_argument("event must be t or b");
    }
    double c;
    if (sender.speed && receiver.speed) {
        c = 100 * 1000000;
    } else {
        c = 5 * 1000000;
    }
    double d_mean = 96 * 1000 / c;

    random_device rd;
    mt19937 rnd_gen (rd ());
    exponential_distribution<double> distribution (1/d_mean);
    
    double d = distribution(gen);

    double l_ij = prop_delay + m / c + d;

    return l_ij;
}

int main() {
   // Example usage:
    Node sender;
    sender.peer_id = 1;
    sender.cpu = 1;
    sender.speed = 1; // Example node with CPU 1 and speed 1

    Node receiver;
    receiver.peer_id = 2;
    receiver.cpu = 0;
    receiver.speed = 1; // Example node with CPU 0 and speed 1

    double prop_delay = 0.2; // Example prop_delay value

    mt19937 gen(time(0));
    double l_ij = latency(sender, receiver, 't', prop_delay, gen);
    std::cout << "Latency for transaction: " << l_ij << " seconds" << std::endl;

    // Assuming you have defined the Block class somewhere and created a block object
    // Block block(10); // Example block with 10 transactions

    // l_ij = latency(sender, receiver, 'b', prop_delay, &block);
    // std::cout << "Latency for block: " << l_ij << " seconds" << std::endl;

    return 0;
}