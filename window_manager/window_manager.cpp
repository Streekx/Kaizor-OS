#include <iostream>
#include <vector>
#include "window.h"

using namespace std;

class WindowManager {

private:
    vector<Window> windows;
    int nextId = 1;

public:

    void createWindow(string title) {
        Window w;
        w.id = nextId++;
        w.title = title;
        w.x = 100;
        w.y = 100;
        w.width = 400;
        w.height = 300;
        w.focused = true;

        windows.push_back(w);

        cout << "[WM] Window Created: " << title << endl;
    }

    void focusWindow(int id) {
        for (auto &w : windows)
            w.focused = (w.id == id);

        cout << "[WM] Focus changed to window " << id << endl;
    }

    void moveWindow(int id, int x, int y) {
        for (auto &w : windows) {
            if (w.id == id) {
                w.x = x;
                w.y = y;
            }
        }
        cout << "[WM] Window moved" << endl;
    }

    void render() {
        cout << "\n[WM] Rendering Windows..." << endl;

        for (auto &w : windows) {
            cout << "Window: " << w.title
                 << " Pos(" << w.x << "," << w.y << ")"
                 << (w.focused ? " [FOCUSED]" : "")
                 << endl;
        }
    }
};
