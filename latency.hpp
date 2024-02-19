#ifndef LATENCY_HPP
#define LATENCY_HPP

#include "node.hpp"
#include "transaction.hpp"
#include "task.hpp"
#include <random>

using namespace std;

class Node;

double latency(Node sender, Node receiver, char event, double prop_delay, mt19937& gen);

#endif