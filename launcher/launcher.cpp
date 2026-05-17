#include "launcher.hpp"

Launcher::Launcher() {

    loadDefaultApps();
}

void Launcher::loadDefaultApps() {

    icons.push_back(
        AppIcon(
            "Files",
            60,
            80,
            64
        )
    );

    icons.push_back(
        AppIcon(
            "Browser",
            60,
            180,
            64
        )
    );

    icons.push_back(
        AppIcon(
            "Terminal",
            60,
            280,
            64
        )
    );

    icons.push_back(
        AppIcon(
            "Settings",
            60,
            380,
            64
        )
    );
}

void Launcher::render(
    Renderer& renderer,
    TextRenderer& textRenderer,
    TTF_Font* font
) {

    for (
        auto& icon : icons
    ) {

        /* ICON */

        renderer.drawRect(
            icon.x,
            icon.y,
            icon.size,
            icon.size,
            Color(
                90,
                140,
                255
            )
        );

        /* LABEL */

        textRenderer.drawText(
            renderer,
            font,
            icon.name,
            icon.x,
            icon.y + 74,
            SDL_Color{
                255,
                255,
                255,
                255
            }
        );
    }
}
