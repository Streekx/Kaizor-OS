#include <iostream>

#include "network_manager.h"

using namespace std;

void NetworkManager::init() {

    cout << "[NETWORK] Manager Started"
         << endl;
}

void NetworkManager::connect() {

    cout << "[NETWORK] Connected"
         << endl;
}

void NetworkManager::disconnect() {

    cout << "[NETWORK] Disconnected"
         << endl;
}
