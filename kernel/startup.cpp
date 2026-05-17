#include <iostream>

#include "startup.h"

using namespace std;

void Startup::bootSequence() {

    cout << "[STARTUP] Boot Sequence"
         << endl;
}

void Startup::loadDesktop() {

    cout << "[STARTUP] Desktop Runtime Loaded"
         << endl;
}
