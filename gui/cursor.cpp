#include <iostream>

#include "cursor.h"

using namespace std;

Cursor::Cursor() {

    x = 0;

    y = 0;
}

void Cursor::move(
    int newX,
    int newY
) {

    x = newX;

    y = newY;

    cout << "[CURSOR] Move -> ("
         << x
         << ","
         << y
         << ")"
         << endl;
}

void Cursor::render() {

    cout << "[CURSOR] Rendered"
         << endl;
}
