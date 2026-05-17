#include "window.h"

Window::Window(
    int id,
    std::string title,
    int x,
    int y,
    int width,
    int height
) {

    this->id = id;

    this->title = title;

    this->x = x;

    this->y = y;

    this->width = width;

    this->height = height;

    this->focused = false;
}
