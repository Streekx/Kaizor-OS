#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <vector>
#include "window.h"

class WindowManager {

private:

    std::vector<Window> windows;

public:

    void createWindow(
        int id,
        std::string title,
        int x,
        int y,
        int width,
        int height
    );

    void render();
};

#endif
