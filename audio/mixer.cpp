#include <iostream>

#include "mixer.h"

using namespace std;

void Mixer::load() {

    cout << "[MIXER] Loaded"
         << endl;
}

void Mixer::volume(
    int level
) {

    cout << "[MIXER] Volume "
         << level
         << "%"
         << endl;
}
