# P2P Blockchain Network Simulation

A P2P Blockchain Network Simulation as part of CS 765 : Blockchain, Cryptocurrencies and Smart Contracts at IIT Bombay

## Getting Started
You must have Graphviz installed over your system to be able to print the network topology. Check this link to download a version compatible to your system - https://graphviz.org/download/

### Clone the repository
```
git clone https://github.com/ariel-waisburg/p2p-network-simulator
cd p2p-network-simulator
```

### Executing program

```
g++ main.cpp -o simulation.exe
./simulation.exe
```

### Enter parameters

```
Enter the number of peers in the network: 11

Mean time of block interarrival in seconds: 600

Mean number of transaction per second: 0.05

Duration for the simulation to run in seconds: 4000

Enter the percentage of slow peers in the network (%): 45

Enter the percentage of low cpu in the network (%): 55
```

### Analyze Results
- Network topology visualization -> graph.png

- Simulation dump -> simulation_output.txt

## Authors

 - [Arjun Gupta](https://github.com/Arjupta)
 - [Rayane Tayache](https://github.com/Rayane-T)
 - [Ariel Waisburg](https://github.com/ariel-waisburg)
