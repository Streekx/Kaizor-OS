#pragma once

#include <vector>

#include <SDL2/SDL.h>

#include "../graphics/renderer.hpp"

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
        Renderer& renderer
    );

    void handleEvents(
        SDL_Event& event
    );
};
