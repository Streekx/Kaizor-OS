#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

enum class LoginPhase {
    FADE_IN,
    IDLE,
    AUTHENTICATING,
    SUCCESS,
    FADE_OUT,
    DONE,
    POWER_MENU
};

class LoginManager {
private:
    int         screenW, screenH;
    LoginPhase  phase;
    int         frame;
    float       fadeAlpha;
    float       glowPulse;
    float       authTimer;
    float       cardSlide;

    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;

    std::string password;
    bool        showPowerMenu;
    int         mouseX, mouseY;

    int cardX, cardY, cardW, cardH;
    int pwFieldX, pwFieldY, pwFieldW, pwFieldH;
    int loginBtnX, loginBtnY, loginBtnW, loginBtnH;
    int powerBtnX, powerBtnY, powerBtnR;

    bool loginBtnHover;
    bool powerBtnHover;

    void renderBackground(Renderer& r);
    void renderLoginCard(Renderer& r);
    void renderAvatar(Renderer& r, int cx, int cy, int radius);
    void renderPasswordField(Renderer& r);
    void renderLoginButton(Renderer& r);
    void renderPowerButton(Renderer& r);
    void renderPowerMenu(Renderer& r);
    void renderClock(Renderer& r);
    void renderFadeOverlay(Renderer& r);

    void attemptLogin();
    bool hitLoginBtn(int mx, int my) const;
    bool hitPowerBtn(int mx, int my) const;

public:
    LoginManager(int w, int h);

    void setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small);
    void handleEvent(SDL_Event& e);
    void update(float dt);
    void render(Renderer& r);
    bool isComplete() const;
    void setMousePos(int mx, int my);
};
