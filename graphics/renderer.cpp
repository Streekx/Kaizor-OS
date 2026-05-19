#include "renderer.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

Renderer::Renderer() : sdl(nullptr) {}

bool Renderer::initialize(SDL_Window* window) {
    sdl = SDL_CreateRenderer(window, -1,
                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl) {
        std::cout << "[RENDERER] Failed: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_SetRenderDrawBlendMode(sdl, SDL_BLENDMODE_BLEND);
    std::cout << "[RENDERER] Initialized" << std::endl;
    return true;
}

void Renderer::setColor(const Color& c) {
    SDL_SetRenderDrawColor(sdl,
        (Uint8)std::clamp(c.r, 0, 255),
        (Uint8)std::clamp(c.g, 0, 255),
        (Uint8)std::clamp(c.b, 0, 255),
        (Uint8)std::clamp(c.a, 0, 255));
}

void Renderer::clear(Color color) {
    setColor(color);
    SDL_RenderClear(sdl);
}

// ─── Solid Rectangles ─────────────────────────────────────────────────────────

void Renderer::drawRect(int x, int y, int w, int h, Color color) {
    setColor(color);
    SDL_Rect r = {x, y, w, h};
    SDL_RenderFillRect(sdl, &r);
}

void Renderer::drawOutlineRect(int x, int y, int w, int h, Color color, int thickness) {
    setColor(color);
    for (int i = 0; i < thickness; i++) {
        SDL_Rect r = {x + i, y + i, w - 2 * i, h - 2 * i};
        SDL_RenderDrawRect(sdl, &r);
    }
}

// ─── Circles ──────────────────────────────────────────────────────────────────

void Renderer::drawFilledCircle(int cx, int cy, int r, Color color) {
    setColor(color);
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrt((double)(r * r - dy * dy));
        SDL_RenderDrawLine(sdl, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Renderer::drawCircleOutline(int cx, int cy, int r, Color color) {
    setColor(color);
    int x = r, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(sdl, cx + x, cy + y);
        SDL_RenderDrawPoint(sdl, cx + y, cy + x);
        SDL_RenderDrawPoint(sdl, cx - y, cy + x);
        SDL_RenderDrawPoint(sdl, cx - x, cy + y);
        SDL_RenderDrawPoint(sdl, cx - x, cy - y);
        SDL_RenderDrawPoint(sdl, cx - y, cy - x);
        SDL_RenderDrawPoint(sdl, cx + y, cy - x);
        SDL_RenderDrawPoint(sdl, cx + x, cy - y);
        if (err <= 0) { y++; err += 2 * y + 1; }
        if (err > 0)  { x--; err -= 2 * x + 1; }
    }
}

void Renderer::drawFilledEllipse(int cx, int cy, int rx, int ry, Color color) {
    setColor(color);
    for (int dy = -ry; dy <= ry; dy++) {
        float t = (float)dy / ry;
        int dx = (int)(rx * sqrt(1.0f - t * t));
        SDL_RenderDrawLine(sdl, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// ─── Rounded Rectangles ───────────────────────────────────────────────────────

void Renderer::drawRoundedRect(int x, int y, int w, int h, int radius, Color color) {
    if (w <= 0 || h <= 0) return;
    radius = std::min(radius, std::min(w / 2, h / 2));

    // Three fill strips
    drawRect(x + radius, y,          w - 2 * radius, radius,          color);
    drawRect(x,          y + radius, w,               h - 2 * radius, color);
    drawRect(x + radius, y + h - radius, w - 2 * radius, radius,      color);

    // Four corner circles
    drawFilledCircle(x + radius,         y + radius,         radius, color);
    drawFilledCircle(x + w - radius - 1, y + radius,         radius, color);
    drawFilledCircle(x + radius,         y + h - radius - 1, radius, color);
    drawFilledCircle(x + w - radius - 1, y + h - radius - 1, radius, color);
}

void Renderer::drawRoundedOutline(int x, int y, int w, int h, int radius, Color color, int thick) {
    for (int i = 0; i < thick; i++) {
        drawRoundedRect(x + i, y + i, w - 2 * i, h - 2 * i, radius - i,
                        Color(color.r, color.g, color.b, color.a / (thick + 1) * (i + 1)));
    }
    drawRoundedRect(x, y, w, h, radius, Color(color.r, color.g, color.b, 0));
    // Draw just the border by drawing outer and masking inner — use outline approach
    // Simple: draw a non-filled rounded outline using arc segments
    (void)thick;
}

// ─── Gradients ────────────────────────────────────────────────────────────────

void Renderer::drawVerticalGradient(int x, int y, int w, int h, Color top, Color bottom) {
    if (w <= 0 || h <= 0) return;
    for (int i = 0; i < h; i++) {
        float t = (h > 1) ? (float)i / (float)(h - 1) : 0.0f;
        Color c(
            (int)(top.r + t * (bottom.r - top.r)),
            (int)(top.g + t * (bottom.g - top.g)),
            (int)(top.b + t * (bottom.b - top.b)),
            (int)(top.a + t * (bottom.a - top.a))
        );
        setColor(c);
        SDL_RenderDrawLine(sdl, x, y + i, x + w - 1, y + i);
    }
}

void Renderer::drawHorizontalGradient(int x, int y, int w, int h, Color left, Color right) {
    if (w <= 0 || h <= 0) return;
    for (int i = 0; i < w; i++) {
        float t = (w > 1) ? (float)i / (float)(w - 1) : 0.0f;
        Color c(
            (int)(left.r + t * (right.r - left.r)),
            (int)(left.g + t * (right.g - left.g)),
            (int)(left.b + t * (right.b - left.b)),
            (int)(left.a + t * (right.a - left.a))
        );
        setColor(c);
        SDL_RenderDrawLine(sdl, x + i, y, x + i, y + h - 1);
    }
}

void Renderer::drawRoundedVerticalGradient(int x, int y, int w, int h, int radius, Color top, Color bottom) {
    // Approximate: draw gradient then punch rounded corners
    // Simple approach: draw gradient strips and use circle fills at corners
    if (w <= 0 || h <= 0) return;
    radius = std::min(radius, std::min(w / 2, h / 2));

    for (int i = 0; i < h; i++) {
        float t = (h > 1) ? (float)i / (float)(h - 1) : 0.0f;
        Color c(
            (int)(top.r + t * (bottom.r - top.r)),
            (int)(top.g + t * (bottom.g - top.g)),
            (int)(top.b + t * (bottom.b - top.b)),
            (int)(top.a + t * (bottom.a - top.a))
        );
        setColor(c);

        int lx = x, rx = x + w - 1;
        if (i < radius) {
            int dy = radius - i;
            int dx = (int)sqrt((double)(radius * radius - dy * dy));
            lx = x + radius - dx;
            rx = x + w - radius + dx - 1;
        } else if (i >= h - radius) {
            int dy = i - (h - radius - 1);
            int dx = (int)sqrt((double)(radius * radius - dy * dy));
            lx = x + radius - dx;
            rx = x + w - radius + dx - 1;
        }
        if (rx >= lx)
            SDL_RenderDrawLine(sdl, lx, y + i, rx, y + i);
    }
}

// ─── Lines ────────────────────────────────────────────────────────────────────

void Renderer::drawLine(int x1, int y1, int x2, int y2, Color color) {
    setColor(color);
    SDL_RenderDrawLine(sdl, x1, y1, x2, y2);
}

void Renderer::drawThickLine(int x1, int y1, int x2, int y2, int thickness, Color color) {
    for (int i = -thickness / 2; i <= thickness / 2; i++) {
        setColor(color);
        SDL_RenderDrawLine(sdl, x1, y1 + i, x2, y2 + i);
    }
}

// ─── Textures ─────────────────────────────────────────────────────────────────

void Renderer::drawTexture(SDL_Texture* tex, int x, int y, int w, int h) {
    if (!tex) return;
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(sdl, tex, nullptr, &dst);
}

void Renderer::drawTextureAlpha(SDL_Texture* tex, int x, int y, int w, int h, uint8_t alpha) {
    if (!tex) return;
    SDL_SetTextureAlphaMod(tex, alpha);
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(sdl, tex, nullptr, &dst);
    SDL_SetTextureAlphaMod(tex, 255);
}

// ─── Text ─────────────────────────────────────────────────────────────────────

static SDL_Texture* renderTextToTexture(SDL_Renderer* sdl, TTF_Font* font,
                                         const std::string& text, SDL_Color color,
                                         int& outW, int& outH) {
    if (!font || text.empty()) return nullptr;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return nullptr;
    outW = surf->w;
    outH = surf->h;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(sdl, surf);
    SDL_FreeSurface(surf);
    return tex;
}

void Renderer::drawText(TTF_Font* font, const std::string& text, int x, int y, Color color) {
    if (!font || text.empty()) return;
    SDL_Color c = {(Uint8)color.r, (Uint8)color.g, (Uint8)color.b, (Uint8)color.a};
    int tw = 0, th = 0;
    SDL_Texture* tex = renderTextToTexture(sdl, font, text, c, tw, th);
    if (!tex) return;
    SDL_Rect dst = {x, y, tw, th};
    SDL_RenderCopy(sdl, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Renderer::drawTextShadow(TTF_Font* font, const std::string& text, int x, int y,
                               Color color, Color shadow, int ox, int oy) {
    drawText(font, text, x + ox, y + oy, shadow);
    drawText(font, text, x,      y,      color);
}

void Renderer::drawTextCentered(TTF_Font* font, const std::string& text,
                                 int x, int y, int w, int h, Color color) {
    if (!font || text.empty()) return;
    int tw = measureTextW(font, text);
    int th = measureTextH(font, text);
    drawText(font, text, x + (w - tw) / 2, y + (h - th) / 2, color);
}

void Renderer::drawTextRight(TTF_Font* font, const std::string& text, int right, int y, Color color) {
    if (!font || text.empty()) return;
    int tw = measureTextW(font, text);
    drawText(font, text, right - tw, y, color);
}

int Renderer::measureTextW(TTF_Font* font, const std::string& text) {
    if (!font || text.empty()) return 0;
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text.c_str(), &w, &h);
    return w;
}

int Renderer::measureTextH(TTF_Font* font, const std::string& text) {
    if (!font || text.empty()) return TTF_FontHeight(font);
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text.c_str(), &w, &h);
    return h;
}

// ─── Clipping ─────────────────────────────────────────────────────────────────

void Renderer::pushClip(int x, int y, int w, int h) {
    SDL_Rect r = {x, y, w, h};
    SDL_RenderSetClipRect(sdl, &r);
}

void Renderer::popClip() {
    SDL_RenderSetClipRect(sdl, nullptr);
}

// ─── Frame ────────────────────────────────────────────────────────────────────

void Renderer::present() {
    SDL_RenderPresent(sdl);
}

SDL_Renderer* Renderer::getSDLRenderer() {
    return sdl;
}

void Renderer::shutdown() {
    if (sdl) {
        SDL_DestroyRenderer(sdl);
        sdl = nullptr;
    }
}
