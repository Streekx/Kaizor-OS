#include "app_icon.hpp"

AppIcon::AppIcon(
    const std::string& appName,
    int posX,
    int posY,
    int iconSize
) {

    name = appName;

    x = posX;
    y = posY;

    size = iconSize;
}
