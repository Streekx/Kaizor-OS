#include <iostream>

#include "workspace.h"

using namespace std;

void Workspace::init() {

    cout << "[WORKSPACE] Manager Started"
         << endl;
}

void Workspace::switchWorkspace(int id) {

    cout << "[WORKSPACE] Switched To "
         << id
         << endl;
}
