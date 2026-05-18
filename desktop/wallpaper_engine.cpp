#include "wallpaper_engine.hpp"

WallpaperEngine::WallpaperEngine() {

    wallpaperTexture = nullptr;
}

bool WallpaperEngine::loadWallpaper(
    SDL_Renderer* sdlRenderer
) {

    SDL_Surface* surface = IMG_Load(
        "assets/wallpapers/default.png"
    );

    if (!surface) {

        return false;
    }

    wallpaperTexture = SDL_CreateTextureFromSurface(
        sdlRenderer,
        surface
    );

    SDL_FreeSurface(surface);

    return wallpaperTexture != nullptr;
}

void WallpaperEngine::render(
    Renderer& renderer
) {

    if (!wallpaperTexture) {

        renderer.clear(
            Color(
                12,
                18,
                28
            )
        );

        return;
    }

    SDL_RenderCopy(
        renderer.getSDLRenderer(),
        wallpaperTexture,
        nullptr,
        nullptr
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
