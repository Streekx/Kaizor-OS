#pragma once

#include <string>
#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

class Window {
private:
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

    // Getters
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    const std::string& getTitle() const;
};
