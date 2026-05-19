#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"

class AppLauncher {
private:
    bool  m_open;
    float m_fraction;   // 0.0 = closed, 1.0 = fully open
    int   m_mouseX, m_mouseY;

public:
    static const int PANEL_W = 680;
    static const int PANEL_H = 514;

    AppLauncher();

    bool  isOpen()   const { return m_open || m_fraction > 0.0f; }

    void  toggle();
    void  open();
    void  close();

    void  update();
    void  setMousePos(int mx, int my);
    void  handleEvent(SDL_Event& e);
    void  render(Renderer& r, TTF_Font* font, TTF_Font* smallFont);
};
