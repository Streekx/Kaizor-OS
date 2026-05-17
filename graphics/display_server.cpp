#include "display_server.hpp"

#include <iostream>

DisplayServer::DisplayServer() {

    window = nullptr;
}

bool DisplayServer::initialize(
    const std::string& title,
    int width,
    int height
) {

    if (
        SDL_Init(
            SDL_INIT_VIDEO |
            SDL_INIT_AUDIO
        ) < 0
    ) {

        std::cout
            << "[SDL] Init Failed"
            << std::endl;

        return false;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {

        std::cout
            << "[SDL] Window Creation Failed"
            << std::endl;

        return false;
    }

    std::cout
        << "[DISPLAY] Window Created"
        << std::endl;

    return true;
}

SDL_Window* DisplayServer::getWindow() {

    return window;
}

void DisplayServer::shutdown() {

    SDL_DestroyWindow(window);

    SDL_Quit();

    std::cout
        << "[DISPLAY] Shutdown"
        << std::endl;
}
