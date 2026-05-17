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

    /* FIRST WINDOW AUTO FOCUS */

    if (
        windows.size() == 1
    ) {

        windows[0].focused = true;

        focusedWindow = 0;
    }
}

void WindowManager::render(
    Renderer& renderer,
    TextRenderer& textRenderer,
    TTF_Font* font
) {

    for (
        auto& window : windows
    ) {

        /* =========================
           SHADOW
           ========================= */

        renderer.drawRect(
            window.x + 10,
            window.y + 10,
            window.width,
            window.height,
            Color(
                0,
                0,
                0,
                110
            )
        );

        /* =========================
           WINDOW BODY
           ========================= */

        if (
            window.focused
        ) {

            renderer.drawRect(
                window.x,
                window.y,
                window.width,
                window.height,
                UITheme::focusedWindow()
            );
        }

        else {

            renderer.drawRect(
                window.x,
                window.y,
                window.width,
                window.height,
                UITheme::normalWindow()
            );
        }

        /* =========================
           TITLEBAR
           ========================= */

        renderer.drawRect(
            window.x,
            window.y,
            window.width,
            36,
            UITheme::titlebar()
        );

        /* =========================
           WINDOW TITLE
           ========================= */

        textRenderer.drawText(
            renderer,
            font,
            window.title,
            window.x + 14,
            window.y + 9,
            SDL_Color{
                255,
                255,
                255,
                255
            }
        );

        /* =========================
           CLOSE BUTTON
           ========================= */

        renderer.drawRect(
            window.x +
            window.width -
            24,

            window.y +
            12,

            10,
            10,

            Color(
                255,
                95,
                95
            )
        );

        /* =========================
           MINIMIZE BUTTON
           ========================= */

        renderer.drawRect(
            window.x +
            window.width -
            44,

            window.y +
            12,

            10,
            10,

            Color(
                255,
                220,
                90
            )
        );

        /* =========================
           MAXIMIZE BUTTON
           ========================= */

        renderer.drawRect(
            window.x +
            window.width -
            64,

            window.y +
            12,

            10,
            10,

            Color(
                90,
                255,
                120
            )
        );
    }
}

void WindowManager::handleEvents(
    SDL_Event& event
) {

    /* =========================
       MOUSE PRESS
       ========================= */

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

            bool insideTitlebar =

                mouseX >= window.x &&
                mouseX <= window.x + window.width &&

                mouseY >= window.y &&
                mouseY <= window.y + 36;

            if (
                insideTitlebar
            ) {

                /* REMOVE OLD FOCUS */

                for (
                    auto& w :
                    windows
                ) {

                    w.focused =
                        false;
                }

                /* NEW FOCUS */

                window.focused =
                    true;

                focusedWindow =
                    i;

                /* START DRAG */

                window.dragging =
                    true;

                window.dragOffsetX =

                    mouseX -
                    window.x;

                window.dragOffsetY =

                    mouseY -
                    window.y;

                break;
            }
        }
    }

    /* =========================
       MOUSE RELEASE
       ========================= */

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

    /* =========================
       MOUSE MOVE
       ========================= */

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
