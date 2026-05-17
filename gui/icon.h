#ifndef ICON_H
#define ICON_H

#include <string>

class Icon {

public:

    std::string name;

    Icon(
        std::string name
    );

    void render();
};

#endif
