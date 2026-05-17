#include "window_manager.hpp"

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
    Renderer& renderer
) {

    for (
        auto& window : windows
    ) {

        /* SHADOW */

        renderer.drawRect(
            window.x + 8,
            window.y + 8,
            window.width,
            window.height,
            Color(
                0,
                0,
                0,
                120
            )
        );

        /* WINDOW BODY */

        if (window.focused) {

            renderer.drawRect(
                window.x,
                window.y,
                window.width,
                window.height,
                Color(
                    70,
                    90,
                    140
                )
            );
        }

        else {

            renderer.drawRect(
                window.x,
                window.y,
                window.width,
                window.height,
                Color(
                    55,
                    65,
                    90
                )
            );
        }

        /* TITLEBAR */

        renderer.drawRect(
            window.x,
            window.y,
            window.width,
            32,
            Color(
                35,
                40,
                55
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
            int i =
            windows.size() - 1;

            i >= 0;

            i--
        ) {

            Window& window =
                windows[i];

            bool inside =
                mouseX >= window.x &&
                mouseX <= window.x + window.width &&
                mouseY >= window.y &&
                mouseY <= window.y + 32;

            if (inside) {

                focusedWindow =
                    i;

                for (
                    auto& w :
                    windows
                ) {

                    w.focused = false;
                }

                window.focused =
                    true;

                window.dragging =
                    true;

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

            window.dragging =
                false;
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
