#ifndef LATENCY_HPP
#define LATENCY_HPP

#include "node.hpp"
#include <random>
#include <cmath>

double latency(Node sender, Node receiver, char event, double prop_delay, mt19937& gen);

#endif
