#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "color.hpp"

class Renderer {
private:
    SDL_Renderer* sdl;

    void setColor(const Color& c);

public:
    Renderer();

    bool initialize(SDL_Window* window);

    // ─── Clear ────────────────────────────────────────────────
    void clear(Color color = Color(5, 8, 18));

    // ─── Solid Shapes ─────────────────────────────────────────
    void drawRect(int x, int y, int w, int h, Color color);
    void drawOutlineRect(int x, int y, int w, int h, Color color, int thickness = 1);
    void drawRoundedRect(int x, int y, int w, int h, int radius, Color color);
    void drawRoundedOutline(int x, int y, int w, int h, int radius, Color color, int thick = 1);

    // ─── Gradients ────────────────────────────────────────────
    void drawVerticalGradient(int x, int y, int w, int h, Color top, Color bottom);
    void drawHorizontalGradient(int x, int y, int w, int h, Color left, Color right);
    void drawRoundedVerticalGradient(int x, int y, int w, int h, int radius, Color top, Color bottom);

    // ─── Circles ──────────────────────────────────────────────
    void drawFilledCircle(int cx, int cy, int r, Color color);
    void drawCircleOutline(int cx, int cy, int r, Color color);
    void drawFilledEllipse(int cx, int cy, int rx, int ry, Color color);

    // ─── Lines ────────────────────────────────────────────────
    void drawLine(int x1, int y1, int x2, int y2, Color color);
    void drawThickLine(int x1, int y1, int x2, int y2, int thickness, Color color);

    // ─── Textures ─────────────────────────────────────────────
    void drawTexture(SDL_Texture* tex, int x, int y, int w, int h);
    void drawTextureAlpha(SDL_Texture* tex, int x, int y, int w, int h, uint8_t alpha);

    // ─── Text ─────────────────────────────────────────────────
    void drawText(TTF_Font* font, const std::string& text, int x, int y, Color color);
    void drawTextShadow(TTF_Font* font, const std::string& text, int x, int y, Color color, Color shadow, int ox = 1, int oy = 1);
    void drawTextCentered(TTF_Font* font, const std::string& text, int x, int y, int w, int h, Color color);
    void drawTextRight(TTF_Font* font, const std::string& text, int right, int y, Color color);
    int  measureTextW(TTF_Font* font, const std::string& text);
    int  measureTextH(TTF_Font* font, const std::string& text);

    // ─── Clipping ─────────────────────────────────────────────
    void pushClip(int x, int y, int w, int h);
    void popClip();

    // ─── Frame ────────────────────────────────────────────────
    void present();
    SDL_Renderer* getSDLRenderer();
    void shutdown();
};
