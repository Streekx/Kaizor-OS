#pragma once
#include "../graphics/color.hpp"

class UITheme {
public:
    // Wallpaper
    static Color wallpaper();

    // Window colors
    static Color focusedWindow();
    static Color normalWindow();

    // Titlebar
    static Color titlebar();

    // Taskbar
    static Color taskbar();

    // Dock
    static Color dock();

    // Accent colors
    static Color accentBlue();
    static Color accentPurple();
    static Color accentCyan();

    // Shadow
    static Color shadow();
};
