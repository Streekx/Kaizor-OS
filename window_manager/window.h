#ifndef WINDOW_H
#define WINDOW_H

#include <string>

class Window {

public:

    int id;

    std::string title;

    int x;
    int y;

    int width;
    int height;

    bool focused;

    Window(
        int id,
        std::string title,
        int x,
        int y,
        int width,
        int height
    );
};

#endif
