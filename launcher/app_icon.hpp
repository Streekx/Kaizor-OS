#pragma once

#include <string>

class AppIcon {

public:

    std::string name;

    int x;
    int y;

    int size;

    AppIcon(
        const std::string& appName,
        int posX,
        int posY,
        int iconSize
    );
};
