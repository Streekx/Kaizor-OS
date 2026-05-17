#ifndef DESKTOP_ICON_H
#define DESKTOP_ICON_H

#include <string>

class DesktopIcon {

public:

    std::string name;

    int x;
    int y;

    DesktopIcon(
        std::string name,
        int x,
        int y
    );

    void render();

    void open();
};

#endif
