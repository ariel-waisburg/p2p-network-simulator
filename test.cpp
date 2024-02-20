#include <iostream>
#include "models.cpp"
#include "functions.cpp"  // Include the file containing your functions

using namespace std;

void testGenerateExponential() {
    double mean = 10.0; // Example mean value for the exponential distribution
    double result = generateExponential(mean);
    cout << "Generated exponential value: " << result << endl;
}

void testGenerateRandom() {
    int min = 1; // Example min value for the random range
    int max = 100; // Example max value for the random range
    int result = generateRandom(min, max);
    cout << "Generated random value: " << result << endl;
}

void testLatency() {
    // Create mock Node objects for sender and receiver
    Node sender, receiver;
    sender.speed = true; // Example speed for sender
    receiver.speed = true; // Example speed for receiver
    sender.blockchain.push_back(Block()); // Example blockchain for sender

    // Example values for prop_delay and event
    char event = 't'; // 't' for transaction, 'b' for block
    static mt19937 gen(rand());
    uniform_real_distribution<double> distribution(0.01, 0.5);

    // Generate a random number
    double prop_delay = distribution(gen);

    // Generate latency and print the result
    double latencyValue = latency(sender, receiver, event, prop_delay, gen);
    cout << "Latency: " << latencyValue << " milliseconds" << endl;
}

int main() {
    // Test each function
    cout << "Testing generateExponential:" << endl;
    testGenerateExponential();
    cout << endl;

    cout << "Testing generateRandom:" << endl;
    testGenerateRandom();
    cout << endl;

    cout << "Testing latency:" << endl;
    testLatency();
    cout << endl;

    return 0;
}
