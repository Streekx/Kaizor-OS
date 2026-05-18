#include "window.hpp"

Window::Window(
    int winId,
    const std::string& winTitle,
    int posX,
    int posY,
    int w,
    int h
)
    : id(winId),
      title(winTitle),
      x(posX),
      y(posY),
      width(w),
      height(h),
      focused(false) {
}

bool Window::contains(int mx, int my) const {
    return (mx >= x && mx <= x + width &&
            my >= y && my <= y + height);
}

void Window::setFocused(bool value) {
    focused = value;
}

void Window::render(Renderer& renderer, const Color& bg) {
    renderer.drawRect(x, y, width, height, bg);

    // Titlebar
    renderer.drawRect(x, y, width, 32, Color(18, 22, 42, 255));

    // Border (simple)
    renderer.drawRect(x, y, width, height, Color(0, 0, 0, 120));
}

int Window::getX() const { return x; }
int Window::getY() const { return y; }
int Window::getWidth() const { return width; }
int Window::getHeight() const { return height; }
const std::string& Window::getTitle() const { return title; }
