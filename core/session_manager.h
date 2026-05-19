#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

enum class SessionPhase {
    FADE_IN,
    LOADING,
    FADE_OUT,
    DONE
};

struct SessionService {
    std::string name;
    float       progress;
    float       speed;
    bool        complete;
};

class SessionManager {
private:
    int           screenW, screenH;
    SessionPhase  phase;
    int           frame;
    float         fadeAlpha;
    float         globalProgress;

    TTF_Font* fontLarge;
    TTF_Font* fontMedium;
    TTF_Font* fontSmall;

    std::vector<SessionService> services;
    int currentService;

    void renderBackground(Renderer& r);
    void renderProgressBars(Renderer& r);
    void renderStatusText(Renderer& r);
    void renderFadeOverlay(Renderer& r);
    void renderProgressBar(Renderer& r, int x, int y, int w, int h,
                           float progress, const std::string& label, bool active);

public:
    SessionManager(int w, int h);

    void setFonts(TTF_Font* large, TTF_Font* medium, TTF_Font* small);
    void update(float dt);
    void render(Renderer& r);
    bool isComplete() const;
};
