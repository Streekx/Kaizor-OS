#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

enum class SplashPhase {
    FADE_IN,
    LOGO_APPEAR,
    KERNEL_LOGS,
    LOADING_DOTS,
    FADE_OUT,
    DONE
};

class SplashScreen {
private:
    int         screenW, screenH;
    int         frame;
    SplashPhase phase;

    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;

    std::vector<std::string> logLines;
    int   visibleLogs;

    float fadeAlpha;
    float logoAlpha;
    float logoScale;
    float glowPulse;

    void updatePhase();

    void renderBackground(Renderer& r);
    void renderGlowRings(Renderer& r, int cx, int cy, int baseR, float alpha);
    void renderLogo(Renderer& r);
    void renderKLetter(Renderer& r, int cx, int cy, int size, uint8_t alpha);
    void renderKernelLogs(Renderer& r);
    void renderLoadingDots(Renderer& r);
    void renderFadeOverlay(Renderer& r);

public:
    SplashScreen(int w, int h);

    void setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small);
    void update();
    void render(Renderer& r);
    bool finished() const;
};
