# Should be integrated and executed each time a txn or a block is generated or received by a peer.
import random

prop_delay = random.uniform(0.01, 0.5) # should be run at the start of the simulation, unit it's seconds

def latency(sender, receiver, event):    
    # m => still need to review how this should be calculated
    if event == "t":
        m = 0
    elif event == "b":
        m = 8 * 1,000,000
    else:
        raise ValueError("event must be t or b")
    # c_ij
    if sender.is_fast and receiver.is_fast:
        c = 100 * 1,000,000
    else:
        c = 5 * 1,000,000
    # d_ij
    d_mean = 96 * 1000 / c
    d = random.expovariate(1 / d_mean)
    
    l_ij = prop_delay + m/c + d
    
    return l_ij