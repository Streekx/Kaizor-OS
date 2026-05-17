#include <iostream>

#include "sound_driver.h"

using namespace std;

void SoundDriver::detect() {

    cout << "[SOUND DRIVER] Device Found"
         << endl;
}

void SoundDriver::initialize() {

    cout << "[SOUND DRIVER] Initialized"
         << endl;
}
