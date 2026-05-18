#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../graphics/renderer.hpp"

class WallpaperEngine {

private:

    SDL_Texture* wallpaperTexture;

public:

    WallpaperEngine();

    bool loadWallpaper(
        SDL_Renderer* sdlRenderer
    );

    void render(
        Renderer& renderer
    );

    void destroy();
};
