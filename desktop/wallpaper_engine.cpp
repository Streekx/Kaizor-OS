#include "wallpaper_engine.hpp"
#include "../gui/ui_theme.hpp"
#include <iostream>
#include <cmath>

WallpaperEngine::WallpaperEngine() : wallpaperTexture(nullptr) {}

bool WallpaperEngine::loadWallpaper(SDL_Renderer* sdlRenderer) {
    SDL_Surface* surface = IMG_Load("assets/wallpapers/default_wallpaper_kaizor.png");

    if (!surface) {
        std::cout << "[WALLPAPER] File not found, using procedural wallpaper" << std::endl;
        return false;
    }

    wallpaperTexture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
    SDL_FreeSurface(surface);

    if (!wallpaperTexture) {
        std::cout << "[WALLPAPER] Texture creation failed" << std::endl;
        return false;
    }

    std::cout << "[WALLPAPER] Loaded from disk" << std::endl;
    return true;
}

void WallpaperEngine::render(Renderer& r) {
    const int W = 1280, H = 720;

    if (wallpaperTexture) {
        r.drawTexture(wallpaperTexture, 0, 0, W, H);
        r.drawRect(0, 0, W, H, Color(0, 0, 0, 55));
    } else {
        // ─── Procedural gradient wallpaper ────────────────────
        r.drawVerticalGradient(0, 0, W, H,
            Color( 8, 10, 24),   // deep navy top
            Color(14, 18, 38));  // slightly brighter bottom

        // ─── Ambient light blobs ──────────────────────────────

        // Blue glow (left)
        for (int ring = 200; ring >= 20; ring -= 20) {
            int alpha = (int)(18.0f * (1.0f - ring / 220.0f)) + 2;
            r.drawRoundedRect(60 - ring / 2, 80 - ring / 2,
                              ring, ring, ring / 2,
                              Color(60, 100, 255, alpha));
        }

        // Purple glow (right)
        for (int ring = 260; ring >= 20; ring -= 20) {
            int alpha = (int)(16.0f * (1.0f - ring / 280.0f)) + 2;
            r.drawRoundedRect(880 - ring / 2, 140 - ring / 2,
                              ring, ring, ring / 2,
                              Color(150, 70, 255, alpha));
        }

        // Cyan glow (center-bottom)
        for (int ring = 180; ring >= 20; ring -= 20) {
            int alpha = (int)(14.0f * (1.0f - ring / 200.0f)) + 2;
            r.drawRoundedRect(580 - ring / 2, 520 - ring / 2,
                              ring, ring, ring / 2,
                              Color(50, 210, 200, alpha));
        }

        // ─── Subtle star field ────────────────────────────────
        // Deterministic pseudo-random stars
        unsigned int seed = 42;
        auto rng = [&]() { seed = seed * 1664525u + 1013904223u; return seed; };

        for (int i = 0; i < 120; i++) {
            int sx = (int)(rng() % W);
            int sy = (int)(rng() % H);
            int bright = 60 + (int)(rng() % 130);
            int sz = (rng() % 3 == 0) ? 1 : 0;
            if (sy > H - 100) continue; // no stars near dock
            if (sz == 0) {
                r.drawRect(sx, sy, 1, 1, Color(bright, bright, bright + 20, bright));
            } else {
                r.drawFilledCircle(sx, sy, 1, Color(bright, bright, bright + 20, bright));
            }
        }

        // ─── Subtle horizontal scanlines (screen feel) ────────
        for (int i = 0; i < H; i += 4) {
            r.drawLine(0, i, W, i, Color(0, 0, 0, 8));
        }
    }
}

void WallpaperEngine::destroy() {
    if (wallpaperTexture) {
        SDL_DestroyTexture(wallpaperTexture);
        wallpaperTexture = nullptr;
    }
}
