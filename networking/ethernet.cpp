#include <iostream>

#include "ethernet.h"

using namespace std;

void Ethernet::detectCable() {

    cout << "[ETHERNET] Cable Connected"
         << endl;
}

void Ethernet::connect() {

    cout << "[ETHERNET] Connected"
         << endl;
}
