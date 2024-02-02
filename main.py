import click
import simpy
import random
from node import Node

@click.command()
@click.argument('n', type=int)
@click.argument('z_0', type=int)
@click.argument('z_1', type=int)
@click.argument('txn_mean_interval', type=int)
@click.argument('block_mean_interval', type=int)
@click.argument('sim_duration', type=int)

def run(n, z_0, z_1, txn_mean_interval, block_mean_interval, sim_duration):
    env = simpy.Environment()

    # Node creation
    for i in range(n):
        node_speed = 'slow' if i < int(n * (float(z_0) / 100)) else 'fast'
        node_cpu_power = 'low' if i < int(n * (float(z_1) / 100)) else 'high'
        node = Node(i, node_speed, node_cpu_power)
        Node.all.append(node)

    # Node connections
    for node in Node.all:
        node_peers = random.randint(3, 6)
        potential_peers = [peer for peer in Node.all if peer != node]
        node.peers = random.sample(potential_peers, node_peers)
        for peer in node.peers:
            node.connect(peer)
            
    # Simulation

    env.run(until=sim_duration)

if __name__ == '__main__':
    run()