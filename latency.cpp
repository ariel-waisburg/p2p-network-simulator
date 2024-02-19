#include <iostream>
#include <vector>
#include "node.hpp"
// #include "block.hpp" // to be checked with the inclusion of the block class
#include <random>
#include "latency.hpp"


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