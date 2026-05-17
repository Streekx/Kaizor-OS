#include <iostream>

#include "notification_center.h"

using namespace std;

void NotificationCenter::open() {

    cout << "[NOTIFICATION CENTER] Opened"
         << endl;
}

void NotificationCenter::push(
    const char* text
) {

    cout << "[NOTIFICATION] "
         << text
         << endl;
}
