#pragma once
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

class TaskManagerApp {
public:
    int selectedRow;
    TaskManagerApp();
    void handleEvent(SDL_Event& e, int x, int y, int w, int h);
    void update();
    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                int x, int y, int w, int h);
};
