#include "window_manager.hpp"
#include "../gui/ui_theme.hpp"

WindowManager::WindowManager()
    : focusedWindow(-1) {
}

void WindowManager::createWindow(
    int id,
    const std::string& title,
    int x,
    int y,
    int width,
    int height
) {
    windows.emplace_back(id, title, x, y, width, height);

    // Auto focus newest window
    focusedWindow = (int)windows.size() - 1;
}

void WindowManager::handleEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mx = event.button.x;
        int my = event.button.y;

        // focus top-most clicked window
        for (int i = (int)windows.size() - 1; i >= 0; i--) {
            if (windows[i].contains(mx, my)) {
                focusedWindow = i;
                break;
            }
        }
    }
}

void WindowManager::render(
    Renderer& renderer,
    TextRenderer& textRenderer,
    TTF_Font* font
) {
    for (int i = 0; i < (int)windows.size(); i++) {

        bool isFocused = (i == focusedWindow);

        Color bg = isFocused ? UITheme::focusedWindow()
                             : UITheme::normalWindow();

        windows[i].render(renderer, bg);

        // Draw title text (safe)
        if (font != nullptr) {
            textRenderer.drawText(
                renderer,
                font,
                windows[i].getTitle(),
                windows[i].getX() + 14,
                windows[i].getY() + 10,
                Color(255, 255, 255, 240)
            );
        }
    }
}
