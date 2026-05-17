#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>

#include "window.h"

class WindowManager {

private:

    std::vector<Window> windows;

    int focusedWindow;

public:

    WindowManager();

    void createWindow(
        int id,
        std::string title,
        int x,
        int y,
        int width,
        int height
    );

    void focusWindow(int id);

    void moveWindow(
        int id,
        int newX,
        int newY
    );

    void renderWindows();
};

#endif
