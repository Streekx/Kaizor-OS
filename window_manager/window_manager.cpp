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

    if (
        windows.size() == 1
    ) {

        windows[0].focused = true;

        focusedWindow = 0;
    }
}

void WindowManager::render(
    Renderer& renderer
) {

    for (
        auto& window : windows
    ) {

        /* SHADOW */

        renderer.drawRect(
            window.x + 10,
            window.y + 10,
            window.width,
            window.height,
            UITheme::shadow()
        );

        /* BODY */

        renderer.drawRect(
            window.x,
            window.y,
            window.width,
            window.height,
            window.focused
                ? UITheme::focusedWindow()
                : UITheme::normalWindow()
        );

        /* TITLEBAR */

        renderer.drawRect(
            window.x,
            window.y,
            window.width,
            42,
            UITheme::titlebar()
        );

        /* CONTROL BUTTONS */

        renderer.drawRect(
            window.x + window.width - 28,
            window.y + 14,
            10,
            10,
            Color(255,90,90)
        );

        renderer.drawRect(
            window.x + window.width - 48,
            window.y + 14,
            10,
            10,
            Color(255,220,90)
        );

        renderer.drawRect(
            window.x + window.width - 68,
            window.y + 14,
            10,
            10,
            Color(90,255,120)
        );

        /* CONTENT AREA */

        renderer.drawRect(
            window.x + 24,
            window.y + 74,
            window.width - 48,
            window.height - 110,
            Color(
                60,
                80,
                160,
                90
            )
        );
    }
}

void WindowManager::handleEvents(
    SDL_Event& event
) {

    if (
        event.type ==
        SDL_MOUSEBUTTONDOWN
    ) {

        int mouseX =
            event.button.x;

        int mouseY =
            event.button.y;

        for (
            int i = windows.size() - 1;
            i >= 0;
            i--
        ) {

            Window& window =
                windows[i];

            bool insideTitlebar =

                mouseX >= window.x &&
                mouseX <= window.x + window.width &&

                mouseY >= window.y &&
                mouseY <= window.y + 42;

            if (
                insideTitlebar
            ) {

                for (
                    auto& w :
                    windows
                ) {

                    w.focused = false;
                }

                window.focused = true;

                focusedWindow = i;

                window.dragging = true;

                window.dragOffsetX =
                    mouseX - window.x;

                window.dragOffsetY =
                    mouseY - window.y;

                break;
            }
        }
    }

    if (
        event.type ==
        SDL_MOUSEBUTTONUP
    ) {

        for (
            auto& window :
            windows
        ) {

            window.dragging = false;
        }
    }

    if (
        event.type ==
        SDL_MOUSEMOTION
    ) {

        int mouseX =
            event.motion.x;

        int mouseY =
            event.motion.y;

        for (
            auto& window :
            windows
        ) {

            if (
                window.dragging
            ) {

                window.x =
                    mouseX -
                    window.dragOffsetX;

                window.y =
                    mouseY -
                    window.dragOffsetY;
            }
        }
    }
}
