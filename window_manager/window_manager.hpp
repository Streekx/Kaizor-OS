#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

enum class AppType {
    NONE,
    TERMINAL,
    FILES,
    BROWSER,
    SETTINGS
};

class Window {
public:
    static constexpr int TITLEBAR_H = 44;
    static constexpr int MIN_W      = 300;
    static constexpr int MIN_H      = 200;

    int         id;
    std::string title;
    AppType     appType;

    int x, y, width, height;
    int savedX, savedY, savedW, savedH;

    bool focused;
    bool minimized;
    bool maximized;
    bool pendingClose;

    Window(int id, const std::string& title, AppType type,
           int x, int y, int w, int h);

    bool contains(int mx, int my) const;
    bool titlebarContains(int mx, int my) const;
    bool closeContains(int mx, int my) const;
    bool minContains(int mx, int my) const;
    bool maxContains(int mx, int my) const;

    SDL_Rect getContentRect() const;
};

class WindowManager {
private:
    std::vector<Window> windows;

    bool isDragging;
    int  dragIdx;
    int  dragOfsX, dragOfsY;

    int  mouseX, mouseY;

    void drawWindowChrome(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          const Window& win) const;
    void drawAppContent(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                        const Window& win);

public:
    WindowManager();

    void createWindow(int id, const std::string& title, AppType type,
                      int x, int y, int w, int h);

    void handleEvent(SDL_Event& e);
    void update();

    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont);

    const char* getFocusedTitle() const;
    int  getWindowCount() const { return (int)windows.size(); }
};
