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
    TASK_MANAGER,
    SETTINGS,
    NOTES,
    CALENDAR
};

class Window {
public:
    static constexpr int TITLEBAR_H = 42;
    static constexpr int MIN_W      = 360;
    static constexpr int MIN_H      = 240;

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
    bool resizeContains(int mx, int my) const;

    SDL_Rect getContentRect() const;
};

class WindowManager {
private:
    std::vector<Window> windows;

    bool isDragging;
    int  dragIdx;
    int  dragOfsX, dragOfsY;

    bool isResizing;
    int  resizeIdx;
    int  resizeStartX, resizeStartY;
    int  resizeStartW, resizeStartH;

    int  mouseX, mouseY;

    void drawWindowChrome(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                          const Window& win) const;
    void drawAppContent(Renderer& r, TTF_Font* font, TTF_Font* smallFont,
                        const Window& win);

public:
    WindowManager();

    void createWindow(int id, const std::string& title, AppType type,
                      int x, int y, int w, int h);
    void focusOrCreate(int id, const std::string& title, AppType type,
                       int x, int y, int w, int h);

    void handleEvent(SDL_Event& e);
    void update();

    void render(Renderer& r, TTF_Font* font, TTF_Font* smallFont);

    const char* getFocusedTitle() const;
    int  getWindowCount() const { return (int)windows.size(); }
    bool hasWindowOfType(AppType t) const;
};
