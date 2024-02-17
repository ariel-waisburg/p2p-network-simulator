#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "node.hpp"
#include <vector>

class TXN {
    public:
        int txn_id;
        int sender_id;
        int receiver_id;
        int amount;
};

#endif
