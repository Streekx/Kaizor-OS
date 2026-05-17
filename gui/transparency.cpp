#include <iostream>

#include "transparency.h"

using namespace std;

void Transparency::enable() {

    cout << "[TRANSPARENCY] Enabled"
         << endl;
}

void Transparency::opacity(
    int value
) {

    cout << "[TRANSPARENCY] Opacity "
         << value
         << "%"
         << endl;
}
