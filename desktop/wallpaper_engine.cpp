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

    renderer.clear(
        Color(
            5,
            8,
            18
        )
    );

    if (wallpaperTexture) {

        SDL_Rect bg = {
            0,
            0,
            1280,
            720
        };

        SDL_RenderCopy(
            renderer.getSDLRenderer(),
            wallpaperTexture,
            NULL,
            &bg
        );
    }

    /* DARK OVERLAY */

    renderer.drawRect(
        0,
        0,
        1280,
        720,
        Color(
            0,
            0,
            0,
            45
        )
    );

    /* PREMIUM LIGHT BLOBS */

    renderer.drawRoundedRect(
        140,
        120,
        280,
        280,
        140,
        Color(
            80,
            120,
            255,
            22
        )
    );

    renderer.drawRoundedRect(
        820,
        180,
        340,
        340,
        160,
        Color(
            180,
            90,
            255,
            18
        )
    );

    renderer.drawRoundedRect(
        420,
        460,
        260,
        160,
        80,
        Color(
            90,
            255,
            220,
            18
        )
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
