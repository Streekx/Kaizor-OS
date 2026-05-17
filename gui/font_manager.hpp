#pragma once

#include <SDL2/SDL_ttf.h>

class FontManager {

private:

    TTF_Font* font;

public:

    FontManager();

    bool initialize(
        const char* path,
        int size
    );

    TTF_Font* getFont();

    void shutdown();
};
