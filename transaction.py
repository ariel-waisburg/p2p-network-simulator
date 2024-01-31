import time
import random
import hashlib

class Transaction:
    
    def __init__(self, sender_id, receiver_id, amount):
        self.timestamp = time.time()
        self.id = hashlib.sha256(str(random.randint(1, 100)) + str(self.timestamp))
        self.sender_id = sender_id
        self.receiver_id = receiver_id
        self.amount = amount
        self.is_verified = False
    
    def __repr__(self):
        return "Txn %d: %d pays %d %d coins" % (self.id, self.sender_id, self.receiver_id, self.amount)