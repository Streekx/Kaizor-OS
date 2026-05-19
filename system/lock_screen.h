#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

enum class LockPhase {
    WAKE_ANIMATION,
    IDLE,
    TYPING,
    UNLOCKING,
    DONE
};

class LockScreen {
private:
    int       screenW, screenH;
    LockPhase phase;
    int       frame;
    float     glowPulse;
    float     wakeAlpha;

    TTF_Font* fontXL;
    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;

    std::string password;
    int         mouseX, mouseY;
    bool        unlocked;

    int pwFieldX, pwFieldY, pwFieldW, pwFieldH;
    bool pwFieldHover;

    void renderBackground(Renderer& r);
    void renderClock(Renderer& r);
    void renderUnlockCard(Renderer& r);
    void renderPasswordField(Renderer& r);
    void renderHint(Renderer& r);
    void renderWakeOverlay(Renderer& r);

public:
    LockScreen(int w, int h);

    void setFonts(TTF_Font* xl, TTF_Font* large, TTF_Font* medium, TTF_Font* small);
    void handleEvent(SDL_Event& e);
    void update(float dt);
    void render(Renderer& r);
    bool isUnlocked() const;
    void lock();
    void setMousePos(int mx, int my);
};
