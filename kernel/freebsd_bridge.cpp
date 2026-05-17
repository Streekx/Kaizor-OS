#include <iostream>

#include "freebsd_bridge.h"

using namespace std;

void FreeBSDBridge::detectKernel() {

    cout << "[FREEBSD] Kernel Detected"
         << endl;
}

void FreeBSDBridge::mountSystem() {

    cout << "[FREEBSD] Root Filesystem Mounted"
         << endl;
}

void FreeBSDBridge::startUserspace() {

    cout << "[FREEBSD] Userspace Started"
         << endl;
}
