#include "wallpaper_engine.hpp"

#include <iostream>

WallpaperEngine::WallpaperEngine() {

    wallpaperTexture = nullptr;
}

bool WallpaperEngine::loadWallpaper(
    SDL_Renderer* sdlRenderer
) {

    SDL_Surface* surface = IMG_Load(
        "assets/wallpapers/default_wallpaper_kaizor.png"
    );

    if (!surface) {

        std::cout
            << "[WALLPAPER] Failed: "
            << IMG_GetError()
            << std::endl;

        return false;
    }

    wallpaperTexture = SDL_CreateTextureFromSurface(
        sdlRenderer,
        surface
    );

    SDL_FreeSurface(surface);

    if (!wallpaperTexture) {

        std::cout
            << "[WALLPAPER] Texture Failed"
            << std::endl;

        return false;
    }

    std::cout
        << "[WALLPAPER] Loaded"
        << std::endl;

    return true;
}

void WallpaperEngine::render(
    Renderer& renderer
) {

    /* fallback background */

    renderer.clear(
        Color(
            5,
            8,
            20
        )
    );

    if (!wallpaperTexture) {
        return;
    }

    SDL_Rect rect = {
        0,
        0,
        1280,
        720
    };

    SDL_RenderCopy(
        renderer.getSDLRenderer(),
        wallpaperTexture,
        NULL,
        &rect
    );
}

void WallpaperEngine::destroy() {

    if (wallpaperTexture) {

        SDL_DestroyTexture(
            wallpaperTexture
        );

        wallpaperTexture = nullptr;
    }
}
