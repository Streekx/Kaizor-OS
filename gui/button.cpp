#include <iostream>

#include "button.h"

using namespace std;

Button::Button(
    string label,
    int x,
    int y,
    int width,
    int height
) {

    this->label = label;

    this->x = x;

    this->y = y;

    this->width = width;

    this->height = height;
}

void Button::render() {

    cout << "[BUTTON] "
         << label
         << " Rendered At ("
         << x
         << ","
         << y
         << ")"
         << endl;
}

void Button::click() {

    cout << "[BUTTON] "
         << label
         << " Clicked"
         << endl;
}
