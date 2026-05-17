#ifndef BUTTON_H
#define BUTTON_H

#include <string>

class Button {

public:

    std::string label;

    int x;
    int y;

    int width;
    int height;

    Button(
        std::string label,
        int x,
        int y,
        int width,
        int height
    );

    void render();

    void click();
};

#endif
