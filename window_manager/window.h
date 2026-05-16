#pragma once
#include <string>

struct Window {
    int id;
    std::string title;
    int x, y, width, height;
    bool focused;
};
