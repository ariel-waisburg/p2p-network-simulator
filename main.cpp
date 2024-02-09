#include <iostream>

using namespace std;

int main()
{
    int n_peers;

    cout << "Enter the number of peers in the network: ";
    cin >> n_peers;

    return 0;
}

/*
main.o: main.cpp node.hpp transaction.hpp
    ${CC} ${CFLAGS} -c main.cpp
*/