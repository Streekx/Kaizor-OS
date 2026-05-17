#include <iostream>

#include "drag_manager.h"

using namespace std;

void DragManager::dragWindow() {

    cout << "[DRAG] Window Dragging"
         << endl;
}

void DragManager::dropWindow() {

    cout << "[DRAG] Window Dropped"
         << endl;
}
