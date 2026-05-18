#include "window_manager.hpp"

#include "../gui/ui_theme.hpp"

WindowManager::WindowManager() {

    focusedWindow = -1;
}

void WindowManager::createWindow(
    int id,
    const std::string& title,
    int x,
    int y,
    int width,
    int height
) {

    windows.push_back(
        Window(
            id,
            title,
            x,
            y,
            width,
            height
        )
    );
}

void WindowManager::render(
    Renderer& renderer,
    TextRenderer& textRenderer,
    TTF_Font* font
) {

    for (
        auto& window : windows
    ) {

        /* SHADOW */

        renderer.drawRoundedRect(
            window.x + 10,
            window.y + 10,
            window.width,
            window.height,
            18,
            UITheme::shadow()
        );

        /* WINDOW BODY */

        renderer.drawRoundedRect(
            window.x,
            window.y,
            window.width,
            window.height,
            18,
            window.focused
                ? UITheme::focusedWindow()
                : UITheme::normalWindow()
        );

        /* TITLEBAR */

        renderer.drawRoundedRect(
            window.x,
            window.y,
            window.width,
            42,
            18,
            UITheme::titlebar()
        );

        /* TITLE */

        textRenderer.drawText(
            renderer,
            font,
            window.title,
            window.x + 20,
            window.y + 11,
            SDL_Color{
                255,
                255,
                255,
                255
            }
        );

        /* MAC STYLE BUTTONS */

        renderer.drawRect(
            window.x + 14,
            window.y + 14,
            10,
            10,
            Color(
                255,
                95,
                86
            )
        );

        renderer.drawRect(
            window.x + 30,
            window.y + 14,
            10,
            10,
            Color(
                255,
                189,
                46
            )
        );

        renderer.drawRect(
            window.x + 46,
            window.y + 14,
            10,
            10,
            Color(
                39,
                201,
                63
            )
        );

        /* CONTENT AREA */

        renderer.drawRoundedRect(
            window.x + 18,
            window.y + 58,
            window.width - 36,
            window.height - 76,
            12,
            Color(
                55,
                65,
                120,
                180
            )
        );
    }
}

void WindowManager::handleEvents(
    SDL_Event& event
) {

}
