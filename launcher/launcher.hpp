#pragma once

#include <vector>

#include "../graphics/renderer.hpp"

#include "../gui/text_renderer.hpp"

#include "../gui/font_manager.hpp"

#include "../graphics/color.hpp"

#include "app_icon.hpp"

class Launcher {

private:

    std::vector<AppIcon> icons;

public:

    Launcher();

    void loadDefaultApps();

    void render(
        Renderer& renderer,
        TextRenderer& textRenderer,
        TTF_Font* font
    );
};
