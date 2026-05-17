#include <iostream>

#include "icon.h"

using namespace std;

Icon::Icon(
    string name
) {

    this->name = name;
}

void Icon::render() {

    cout << "[ICON] "
         << name
         << " Rendered"
         << endl;
}
