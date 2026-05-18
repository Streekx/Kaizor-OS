#include "dock.hpp"

#include "ui_theme.hpp"

void Dock::render(
    Renderer& renderer
) {

    int dockWidth = 420;
    int dockHeight = 78;

    int x = (1280 - dockWidth) / 2;
    int y = 720 - 96;

    /* SHADOW */

    renderer.drawRect(
        x + 6,
        y + 6,
        dockWidth,
        dockHeight,
        UITheme::shadow()
    );

    /* DOCK BODY */

    renderer.drawRect(
        x,
        y,
        dockWidth,
        dockHeight,
        UITheme::dock()
    );

    /* ICONS */

    int iconX = x + 28;

    for (
        int i = 0;
        i < 6;
        i++
    ) {

        renderer.drawRect(
            iconX,
            y + 18,
            42,
            42,
            UITheme::accentBlue()
        );

        renderer.drawRect(
            iconX + 6,
            y + 24,
            30,
            30,
            UITheme::accentCyan()
        );

        iconX += 62;
    }
}
