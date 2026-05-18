#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL.h>

#include "../graphics/renderer.hpp"
#include "../gui/text_renderer.hpp"

#include "window.hpp"

class WindowManager {
private:
    std::vector<Window> windows;
    int focusedWindow;

public:
    WindowManager();

    void createWindow(
        int id,
        const std::string& title,
        int x,
        int y,
        int width,
        int height
    );

    void render(
        Renderer& renderer,
        TextRenderer& textRenderer,
        TTF_Font* font
    );

    void handleEvents(SDL_Event& event);
};
