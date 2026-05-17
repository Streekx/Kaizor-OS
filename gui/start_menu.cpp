#include <iostream>

#include "start_menu.h"

using namespace std;

StartMenu::StartMenu() {

    opened = false;
}

void StartMenu::open() {

    opened = true;

    cout << "[START MENU] Opened"
         << endl;
}

void StartMenu::close() {

    opened = false;

    cout << "[START MENU] Closed"
         << endl;
}

void StartMenu::render() {

    if (opened) {

        cout << "[START MENU] Rendering"
             << endl;
    }
}
