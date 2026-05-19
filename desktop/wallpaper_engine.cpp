#include "wallpaper_engine.hpp"
#include "../gui/ui_theme.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

WallpaperEngine::WallpaperEngine() : wallpaperTexture(nullptr) {}

bool WallpaperEngine::loadWallpaper(SDL_Renderer* sdlRenderer) {
    // Try multiple formats
    const char* paths[] = {
        "assets/wallpapers/default_wallpaper_kaizor.png",
        "assets/wallpapers/default_wallpaper_kaizor.jpg",
    };
    for (auto path : paths) {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) continue;
        wallpaperTexture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
        SDL_FreeSurface(surface);
        if (wallpaperTexture) {
            std::cout << "[WALLPAPER] Loaded from disk" << std::endl;
            return true;
        }
    }
    std::cout << "[WALLPAPER] Using procedural sunset wallpaper" << std::endl;
    return false;
}

// Interpolate between two heights at fractional x
static int lerpi(int a, int b, float t) {
    return a + (int)((b - a) * t);
}

// Evaluate mountain height at pixel x using control points
static int mountainHeight(const int* pts, int count, int W, int x) {
    float fx = (float)x / W * (count - 1);
    int   idx = (int)fx;
    float t   = fx - idx;
    if (idx >= count - 1) return pts[count - 1];
    return lerpi(pts[idx], pts[idx + 1], t);
}

void WallpaperEngine::render(Renderer& r) {
    const int W = 1280, H = 720;

    if (wallpaperTexture) {
        r.drawTexture(wallpaperTexture, 0, 0, W, H);
        // Subtle dark overlay to darken/tint for UI legibility
        r.drawRect(0, 0, W, H, Color(0, 0, 0, 45));
        return;
    }

    // ══════════════════════════════════════════════════════════
    // PROCEDURAL SUNSET MOUNTAIN + DESERT WALLPAPER
    // Inspired by Zorin OS default wallpaper aesthetic
    // ══════════════════════════════════════════════════════════

    // ── 1. SKY GRADIENT (top → horizon) ──────────────────────
    // Deep indigo-purple → rose → coral-peach
    r.drawVerticalGradient(0,   0, W, 160, Color(22,  8, 68, 255), Color(55, 18, 100, 255));
    r.drawVerticalGradient(0, 160, W, 120, Color(55, 18, 100, 255), Color(130, 40, 110, 255));
    r.drawVerticalGradient(0, 280, W,  90, Color(130, 40, 110, 255), Color(200, 75,  75, 255));
    r.drawVerticalGradient(0, 370, W,  70, Color(200, 75,  75, 255), Color(230, 130,  55, 255));
    r.drawVerticalGradient(0, 440, W,  55, Color(230, 130,  55, 255), Color(240, 160,  70, 255));
    // Horizon band (sky meets land)
    r.drawVerticalGradient(0, 495, W,  40, Color(235, 150,  65, 255), Color(175, 100,  40, 255));

    // ── 2. STARS (upper sky only) ─────────────────────────────
    unsigned int seed = 0xDEADBEEF;
    auto rng = [&]() { seed = seed * 1664525u + 1013904223u; return seed; };
    for (int i = 0; i < 90; i++) {
        int sx     = (int)(rng() % W);
        int sy     = (int)(rng() % 320);
        int bright = 110 + (int)(rng() % 120);
        int sz     = (rng() % 5 == 0) ? 1 : 0;
        Color sc(bright, bright, std::min(bright + 30, 255), bright);
        if (sz == 0) r.drawRect(sx, sy, 1, 1, sc);
        else         r.drawFilledCircle(sx, sy, 1, sc);
    }

    // ── 3. SOFT SUN GLOW at horizon ──────────────────────────
    const int sunX = 820, sunY = 450;
    for (int ring = 240; ring >= 8; ring -= 8) {
        float t = (float)ring / 240.0f;
        int a = (int)(35.0f * (1.0f - t * t));
        r.drawFilledCircle(sunX, sunY, ring,
            Color((int)(255 * (1.0f - t * 0.3f)),
                  (int)(160 * (1.0f - t * 0.5f)),
                  (int)( 40 * (1.0f - t)),
                  a));
    }
    // Sun disc
    r.drawFilledCircle(sunX, sunY, 28, Color(255, 220, 100, 200));
    r.drawFilledCircle(sunX, sunY, 20, Color(255, 240, 160, 220));

    // ── 4. FAR MOUNTAINS (blue-purple, behind near ones) ─────
    // Mountain profile control points (11 pts across 1280px)
    static const int FAR_H[] = {
        490, 380, 310, 360, 280, 340, 300, 360, 320, 400, 490
    };
    static const int FAR_N = 11;
    const int FAR_HORIZON = 510;

    for (int row = 280; row < FAR_HORIZON; row++) {
        for (int col = 0; col < W; col++) {
            int mh = mountainHeight(FAR_H, FAR_N, W, col);
            if (row >= mh) {
                // Vertical gradient: lighter at top, darker at base
                float t = (float)(row - mh) / (FAR_HORIZON - mh);
                int r2 = lerpi(70, 40, t);
                int g2 = lerpi(30, 15, t);
                int b2 = lerpi(115, 70, t);
                r.drawRect(col, row, 1, 1, Color(r2, g2, b2, 255));
            }
        }
    }

    // Snow caps on far mountains
    for (int col = 0; col < W; col++) {
        int mh = mountainHeight(FAR_H, FAR_N, W, col);
        int snowBot = mh + 28;
        for (int row = mh; row < snowBot; row++) {
            float t = (float)(row - mh) / 28.0f;
            int a = (int)(200 * (1.0f - t));
            r.drawRect(col, row, 1, 1, Color(230, 220, 245, a));
        }
    }

    // ── 5. NEAR MOUNTAINS (darker, foreground) ───────────────
    static const int NEAR_H[] = {
        530, 440, 400, 460, 380, 440, 410, 450, 430, 490, 540
    };
    static const int NEAR_N = 11;
    const int NEAR_HORIZON = 540;

    for (int row = 360; row < NEAR_HORIZON; row++) {
        for (int col = 0; col < W; col++) {
            int mh = mountainHeight(NEAR_H, NEAR_N, W, col);
            if (row >= mh) {
                float t = (float)(row - mh) / (NEAR_HORIZON - mh);
                int r2 = lerpi(42, 22, t);
                int g2 = lerpi(18, 8,  t);
                int b2 = lerpi(78, 40, t);
                r.drawRect(col, row, 1, 1, Color(r2, g2, b2, 255));
            }
        }
    }

    // Snow on near mountains
    for (int col = 0; col < W; col++) {
        int mh = mountainHeight(NEAR_H, NEAR_N, W, col);
        int snowBot = mh + 22;
        for (int row = mh; row < snowBot; row++) {
            float t = (float)(row - mh) / 22.0f;
            int a = (int)(220 * (1.0f - t));
            r.drawRect(col, row, 1, 1, Color(240, 232, 255, a));
        }
    }

    // ── 6. LAND BASE (between mountains and dunes) ───────────
    // Dark purple-brown land fill from horizon down
    r.drawVerticalGradient(0, 500, W, 50,
        Color(35, 14, 55, 255), Color(80, 45, 20, 255));

    // ── 7. DESERT DUNES ───────────────────────────────────────
    // Multiple sinusoidal dune layers
    r.drawVerticalGradient(0, 545, W, H - 545,
        Color(120, 65, 22, 255), Color(75, 38, 10, 255));

    struct Dune { float baseY, amp, freq, phase; Color col; };
    static const Dune DUNES[] = {
        { 548, 28, 0.00280f, 0.00f, Color(135, 72, 25, 255) },
        { 572, 35, 0.00360f, 1.80f, Color(150, 84, 30, 255) },
        { 598, 22, 0.00440f, 3.20f, Color(110, 58, 18, 255) },
        { 620, 18, 0.00520f, 0.90f, Color( 95, 48, 14, 255) },
    };

    for (auto& d : DUNES) {
        for (int col = 0; col < W; col++) {
            float dy    = d.baseY + d.amp * sinf(col * d.freq + d.phase);
            int   yTop  = (int)dy;
            if (yTop < H)
                r.drawLine(col, yTop, col, H, d.col);
        }
        // Dune highlight (sun catches the crest)
        for (int col = 0; col < W; col++) {
            float dy   = d.baseY + d.amp * sinf(col * d.freq + d.phase);
            float grad = cosf(col * d.freq + d.phase); // slope
            if (grad > 0.5f) {
                int   yTop = (int)dy;
                int   a    = (int)(50.0f * (grad - 0.5f) * 2.0f);
                for (int i = 0; i < 4 && yTop + i < H; i++)
                    r.drawRect(col, yTop + i, 1, 1,
                        Color(230, 165, 80, a * (4 - i) / 4));
            }
        }
    }

    // ── 8. ATMOSPHERIC HORIZON GLOW ───────────────────────────
    // Warm glow band at the horizon meeting point
    for (int row = 480; row < 530; row++) {
        float t = (float)(row - 480) / 50.0f;
        float bell = 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f;
        if (bell < 0) bell = 0;
        int a = (int)(60 * bell);
        r.drawLine(0, row, W, row, Color(240, 120, 55, a));
    }

    // Pink/magenta glow centered near sun
    for (int ring = 280; ring >= 12; ring -= 12) {
        float t = (float)ring / 280.0f;
        int a   = (int)(20.0f * (1.0f - t * t));
        r.drawFilledCircle(sunX, sunY + 20, ring,
            Color(220, 80, 60, a));
    }

    // ── 9. VIGNETTE (edge darkening for depth) ─────────────────
    for (int i = 0; i < 80; i++) {
        int a = (int)(100.0f * (1.0f - (float)i / 80.0f));
        // Top
        r.drawRect(0, i, W, 1, Color(0, 0, 0, a));
        // Left
        r.drawRect(0, 0, i / 3, H, Color(0, 0, 0, a / 5));
        // Right
        r.drawRect(W - i / 3, 0, i / 3, H, Color(0, 0, 0, a / 5));
    }

    // ── 10. SUBTLE PURPLE CLOUD WISPS ─────────────────────────
    // A few horizontal smeared ellipses in the upper sky
    struct Cloud { int cx, cy, rx, ry; };
    static const Cloud CLOUDS[] = {
        { 180, 95,  120, 22 },
        { 450, 145, 90,  18 },
        { 750, 80,  150, 24 },
        { 1050,130, 110, 20 },
        { 320, 185, 70,  14 },
    };
    for (auto& c : CLOUDS) {
        for (int bx = -c.rx; bx <= c.rx; bx += 2) {
            float fy  = c.ry * sqrtf(std::max(0.0f, 1.0f - (float)(bx*bx)/(c.rx*c.rx)));
            int   iy  = (int)fy;
            float t   = (float)(bx + c.rx) / (c.rx * 2);
            float bell = 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f;
            if (bell < 0) bell = 0;
            int a = (int)(28 * bell);
            r.drawLine(c.cx + bx, c.cy - iy, c.cx + bx, c.cy + iy,
                Color(200, 160, 230, a));
        }
    }
}

void WallpaperEngine::destroy() {
    if (wallpaperTexture) {
        SDL_DestroyTexture(wallpaperTexture);
        wallpaperTexture = nullptr;
    }
}
