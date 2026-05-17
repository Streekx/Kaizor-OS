#include "window.hpp"

Window::Window(
    int windowId,
    const std::string& windowTitle,
    int posX,
    int posY,
    int w,
    int h
) {

    id = windowId;

    title = windowTitle;

    x = posX;
    y = posY;

    width = w;
    height = h;

    focused = false;

    dragging = false;

    dragOffsetX = 0;
    dragOffsetY = 0;
}
