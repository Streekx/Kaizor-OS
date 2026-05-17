#include <iostream>

#include "media.h"

using namespace std;

void Media::play() {

    cout << "[MEDIA] Playing"
         << endl;
}

void Media::pause() {

    cout << "[MEDIA] Paused"
         << endl;
}

void Media::stop() {

    cout << "[MEDIA] Stopped"
         << endl;
}
