#include "font_manager.hpp"

#include <iostream>

FontManager::FontManager() {

    font = nullptr;
}

bool FontManager::initialize(
    const char* path,
    int size
) {

    if (TTF_Init() == -1) {

        std::cout
            << "[FONT] SDL_ttf Init Failed"
            << std::endl;

        return false;
    }

    font = TTF_OpenFont(
        path,
        size
    );

    if (!font) {

        std::cout
            << "[FONT] Failed To Load Font"
            << std::endl;

        return false;
    }

    std::cout
        << "[FONT] Loaded"
        << std::endl;

    return true;
}

TTF_Font* FontManager::getFont() {

    return font;
}

void FontManager::shutdown() {

    TTF_CloseFont(font);

    TTF_Quit();

    std::cout
        << "[FONT] Shutdown"
        << std::endl;
}
