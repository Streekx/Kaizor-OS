#include <iostream>

#include "notifications.hpp"

using namespace std;

void Notifications::push(string message) {

    cout << "[NOTIFICATION] "
         << message
         << endl;
}
