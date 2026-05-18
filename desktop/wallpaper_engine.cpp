#include "wallpaper_engine.hpp"
#include "../gui/ui_theme.hpp"
#include <iostream>

WallpaperEngine::WallpaperEngine()
    : wallpaperTexture(nullptr) {
}

bool WallpaperEngine::loadWallpaper(SDL_Renderer* sdlRenderer) {

    if (wallpaperTexture != nullptr) {
        SDL_DestroyTexture(wallpaperTexture);
        wallpaperTexture = nullptr;
    }

    SDL_Surface* surface = IMG_Load("assets/wallpapers/default_wallpaper_kaizor.png");

    if (!surface) {
        std::cout << "[WALLPAPER] Failed to load wallpaper: " << IMG_GetError() << std::endl;
        return false;
    }

    wallpaperTexture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_FreeSurface(surface);

    if (!wallpaperTexture) {
        std::cout << "[WALLPAPER] Failed to create texture: " << SDL_GetError() << std::endl;
        return false;
    }

    std::cout << "[WALLPAPER] Wallpaper Loaded" << std::endl;
    return true;
}

void WallpaperEngine::render(Renderer& renderer) {

    // If wallpaper image loaded, draw it
    if (wallpaperTexture != nullptr) {
        renderer.drawTexture(wallpaperTexture, 0, 0, 1280, 720);
        return;
    }

    // Fallback solid background (prevents black screen)
    renderer.clear(UITheme::wallpaper());

    // Simple gradient style blocks
    renderer.drawRoundedRect(0, 0, 1280, 720, 0, UITheme::wallpaper());
    renderer.drawRoundedRect(120, 120, 420, 300, 28, Color(60, 140, 255, 55));
    renderer.drawRoundedRect(520, 200, 520, 360, 28, Color(255, 120, 180, 45));
    renderer.drawRoundedRect(220, 460, 420, 200, 28, Color(120, 255, 220, 40));
}

void WallpaperEngine::destroy() {
    if (wallpaperTexture != nullptr) {
        SDL_DestroyTexture(wallpaperTexture);
        wallpaperTexture = nullptr;
    }
}
