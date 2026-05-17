#include <iostream>

#include "titlebar.h"

using namespace std;

Titlebar::Titlebar(
    string title
) {

    this->title = title;
}

void Titlebar::render() {

    cout << "[TITLEBAR] "
         << title
         << endl;
}
