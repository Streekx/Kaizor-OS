#include <iostream>

#include "process_manager.h"

using namespace std;

void ProcessManager::init() {

    cout << "[PROCESS] Manager Started"
         << endl;
}

void ProcessManager::startProcess(
    const char* name
) {

    cout << "[PROCESS] Running: "
         << name
         << endl;
}
