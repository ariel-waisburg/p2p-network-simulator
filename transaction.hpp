#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <vector>

class TXN {
    public:
        int txn_id;
        int sender_id;
        int receiver_id;
        int amount;
        int sender_bal;
};

TXN createTXN(Node miner, int id);

bool verifyTransactions(Block block);

#endif
