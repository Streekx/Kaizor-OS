#include "dock.hpp"

#include "ui_theme.hpp"

void Dock::render(
    Renderer& renderer
) {

    int dockWidth = 500;
    int dockHeight = 86;

    int x = (1280 - dockWidth) / 2;
    int y = 720 - 110;

    /* SHADOW */

    renderer.drawRoundedRect(
        x + 8,
        y + 10,
        dockWidth,
        dockHeight,
        24,
        Color(
            0,
            0,
            0,
            90
        )
    );

    /* GLASS BODY */

    renderer.drawRoundedRect(
        x,
        y,
        dockWidth,
        dockHeight,
        24,
        Color(
            22,
            28,
            48,
            215
        )
    );

    int iconX = x + 26;

    for (
        int i = 0;
        i < 7;
        i++
    ) {

        /* ICON BACK */

        renderer.drawRoundedRect(
            iconX,
            y + 16,
            54,
            54,
            16,
            Color(
                55,
                80,
                170,
                220
            )
        );

        /* INNER GLOW */

        renderer.drawRoundedRect(
            iconX + 8,
            y + 24,
            38,
            38,
            12,
            UITheme::accentCyan()
        );

        iconX += 66;
    }
}
