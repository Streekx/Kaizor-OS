#include "font_manager.hpp"
#include <iostream>

FontManager::FontManager() {
    font = nullptr;
}

bool FontManager::initialize(const char* path, int size) {

    font = TTF_OpenFont(path, size);

    if (!font) {
        std::cout << "[FONT] Failed To Load Font: " << path << std::endl;
        std::cout << "[FONT] Error: " << TTF_GetError() << std::endl;
        return false;
    }

    std::cout << "[FONT] Loaded: " << path << std::endl;
    return true;
}

TTF_Font* FontManager::getFont() {
    return font;
}

void FontManager::shutdown() {

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    std::cout << "[FONT] Shutdown" << std::endl;
}
