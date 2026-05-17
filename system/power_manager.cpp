#include <iostream>

#include "power_manager.h"

using namespace std;

void PowerManager::init() {

    cout << "[POWER] Manager Started"
         << endl;
}

void PowerManager::batteryStatus() {

    cout << "[POWER] Battery 92%"
         << endl;
}

void PowerManager::shutdown() {

    cout << "[POWER] Shutdown Requested"
         << endl;
}
