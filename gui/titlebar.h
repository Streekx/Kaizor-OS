#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <string>

class Titlebar {

public:

    std::string title;

    Titlebar(
        std::string title
    );

    void render();
};

#endif
