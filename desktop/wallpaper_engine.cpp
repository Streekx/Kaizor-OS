#include "dock.hpp"

#include "ui_theme.hpp"

void Dock::render(
    Renderer& renderer
) {

    int dockWidth = 430;
    int dockHeight = 82;

    int x = (1280 - dockWidth) / 2;
    int y = 720 - 104;

    /* SHADOW */

    renderer.drawRoundedRect(
        x + 8,
        y + 8,
        dockWidth,
        dockHeight,
        26,
        UITheme::shadow()
    );

    /* BODY */

    renderer.drawRoundedRect(
        x,
        y,
        dockWidth,
        dockHeight,
        26,
        UITheme::dock()
    );

    /* ICONS */

    int iconX = x + 26;

    for (
        int i = 0;
        i < 6;
        i++
    ) {

        renderer.drawRoundedRect(
            iconX,
            y + 18,
            48,
            48,
            16,
            UITheme::accentBlue()
        );

        renderer.drawRoundedRect(
            iconX + 8,
            y + 26,
            32,
            32,
            10,
            UITheme::accentCyan()
        );

        iconX += 64;
    }
}
