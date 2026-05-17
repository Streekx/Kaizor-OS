#pragma once

#include <SDL2/SDL.h>
#include <string>

class DisplayServer {

private:

    SDL_Window* window;

public:

    DisplayServer();

    bool initialize(
        const std::string& title,
        int width,
        int height
    );

    SDL_Window* getWindow();

    void shutdown();
};
