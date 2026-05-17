#include <iostream>

#include "desktop_icon.h"

using namespace std;

DesktopIcon::DesktopIcon(
    string name,
    int x,
    int y
) {

    this->name = name;

    this->x = x;

    this->y = y;
}

void DesktopIcon::render() {

    cout << "[ICON] "
         << name
         << " At ("
         << x
         << ","
         << y
         << ")"
         << endl;
}

void DesktopIcon::open() {

    cout << "[ICON] Opening "
         << name
         << endl;
}
