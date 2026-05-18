#pragma once

#include "../graphics/renderer.hpp"

#include "wallpaper_engine.hpp"
#include "desktop_icons.hpp"

class DesktopShell {

public:

    WallpaperEngine wallpaper;

    DesktopIcons icons;

public:

    DesktopShell();

    void render(
        Renderer& renderer
    );
};
