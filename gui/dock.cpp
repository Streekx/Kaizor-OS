#include "dock.hpp"

#include "ui_theme.hpp"

void Dock::render(
    Renderer& renderer
) {

    /* DOCK BACKGROUND */

    renderer.drawRect(
        440,
        640,
        400,
        60,
        UITheme::dock()
    );

    /* APP ICONS */

    int iconX = 470;

    for (
        int i = 0;
        i < 5;
        i++
    ) {

        renderer.drawRect(
            iconX,
            652,
            36,
            36,
            UITheme::accent()
        );

        iconX += 70;
    }
}
