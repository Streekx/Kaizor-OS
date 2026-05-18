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
        int winId,
        const std::string& winTitle,
        int posX,
        int posY,
        int w,
        int h
    ) {

        id = winId;
        title = winTitle;

        x = posX;
        y = posY;

        width = w;
        height = h;

        focused = false;
        dragging = false;

        dragOffsetX = 0;
        dragOffsetY = 0;
    }
};
