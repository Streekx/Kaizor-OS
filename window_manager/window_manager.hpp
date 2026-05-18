#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"
#include "../gui/text_renderer.hpp"

class Window {
public:
    int id;
    std::string title;

    int x;
    int y;
    int width;
    int height;

    bool focused;

public:
    Window(
        int winId,
        const std::string& winTitle,
        int posX,
        int posY,
        int w,
        int h
    );

    bool contains(int mx, int my) const;

    void setFocused(bool value);

    void render(Renderer& renderer, const Color& bg);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    const std::string& getTitle() const;
};

class WindowManager {
private:
    std::vector<Window> windows;
    int focusedWindow;

public:
    WindowManager();

    void createWindow(
        int id,
        const std::string& title,
        int x,
        int y,
        int width,
        int height
    );

    void render(
        Renderer& renderer,
        TextRenderer& textRenderer,
        TTF_Font* font
    );

    void handleEvents(SDL_Event& event);
};
