#include <iostream>

#include "notifications.h"

using namespace std;

void Notifications::push(string message) {

    cout << "[NOTIFICATION] "
         << message
         << endl;
}
