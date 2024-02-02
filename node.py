class Node():
    
    all = []
    
    def __init__(self, id, speed, cpu_power):
        
        self.id = id
        self.speed = speed
        self.cpu_power = cpu_power
        
        self.utxo = 0
        self.peers = [] 
        self.blockchain = {}
        self.transactions = {}
    
    def connect(self, peer):
        if peer not in self.peers:
            self.peers.append(peer)
            if self not in peer.peers:
                peer.connect(self)
    
    # genesis block
    # generate_transaction
    # broadcast_transaction
    # mining_block
    # broadcast_block
    # receive_block
        # valid transaction
    # update utxo