#include <iostream>

#include "window_manager.h"

using namespace std;

WindowManager::WindowManager() {

    focusedWindow = -1;
}

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
         << title
         << endl;
}

void WindowManager::focusWindow(
    int id
) {

    focusedWindow = id;

    cout << "[WM] Focus Window: "
         << id
         << endl;
}

void WindowManager::moveWindow(
    int id,
    int newX,
    int newY
) {

    for (auto &w : windows) {

        if (w.id == id) {

            w.x = newX;

            w.y = newY;

            cout << "[WM] Moved "
                 << w.title
                 << " -> ("
                 << newX
                 << ","
                 << newY
                 << ")"
                 << endl;
        }
    }
}

void WindowManager::renderWindows() {

    cout << endl;

    cout << "====== WINDOW RENDER ======"
         << endl;

    for (auto &w : windows) {

        cout << "[DRAW] "
             << w.title
             << " Pos("
             << w.x
             << ","
             << w.y
             << ") Size("
             << w.width
             << "x"
             << w.height
             << ")";

        if (w.id == focusedWindow) {

            cout << " [FOCUSED]";
        }

        cout << endl;
    }

    cout << "==========================="
         << endl;
}
