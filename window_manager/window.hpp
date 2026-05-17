#pragma once

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

    bool dragging;

    int dragOffsetX;
    int dragOffsetY;

    Window(
        int windowId,
        const std::string& windowTitle,
        int posX,
        int posY,
        int w,
        int h
    );
};
