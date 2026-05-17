#include <iostream>

#include "window_manager.h"

using namespace std;

void WindowManager::createWindow(
    int id,
    string title,
    int x,
    int y,
    int width,
    int height
) {

    Window w(
        id,
        title,
        x,
        y,
        width,
        height
    );

    windows.push_back(w);

    cout << "[WM] Window Created: "
         << title << endl;
}

void WindowManager::render() {

    cout << endl;

    cout << "====== WINDOWS ======" << endl;

    for (auto &w : windows) {

        cout << w.title
             << " Pos("
             << w.x
             << ","
             << w.y
             << ") Size("
             << w.width
             << "x"
             << w.height
             << ")"
             << endl;
    }

    cout << "=====================" << endl;
}
