#include <iostream>

#include "hardware.h"

using namespace std;

void Hardware::detectCPU() {

    cout << "[HARDWARE] CPU Detected"
         << endl;
}

void Hardware::detectGPU() {

    cout << "[HARDWARE] GPU Detected"
         << endl;
}

void Hardware::detectRAM() {

    cout << "[HARDWARE] RAM Detected"
         << endl;
}
