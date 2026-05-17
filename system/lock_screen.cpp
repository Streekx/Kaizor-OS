#include <iostream>

#include "lock_screen.h"

using namespace std;

void LockScreen::lock() {

    cout << "[LOCK SCREEN] Locked"
         << endl;
}

void LockScreen::unlock() {

    cout << "[LOCK SCREEN] Unlocked"
         << endl;
}
