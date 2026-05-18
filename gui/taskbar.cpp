#include "taskbar.hpp"

#include "ui_theme.hpp"

void Taskbar::render(
    Renderer& renderer
) {

    /* GLASS BAR */

    renderer.drawRoundedRect(
        10,
        10,
        1260,
        42,
        14,
        Color(
            18,
            22,
            38,
            210
        )
    );

    /* LEFT LOGO */

    renderer.drawRoundedRect(
        22,
        18,
        18,
        18,
        6,
        UITheme::accentBlue()
    );

    /* SEARCH */

    renderer.drawRoundedRect(
        70,
        16,
        240,
        22,
        10,
        Color(
            40,
            46,
            72,
            200
        )
    );

    /* RIGHT STATUS */

    renderer.drawRoundedRect(
        1120,
        16,
        120,
        22,
        10,
        Color(
            40,
            46,
            72,
            200
        )
    );

    /* STATUS ICONS */

    renderer.drawRect(
        1140,
        22,
        8,
        8,
        UITheme::accentCyan()
    );

    renderer.drawRect(
        1160,
        22,
        8,
        8,
        UITheme::accentPurple()
    );

    renderer.drawRect(
        1180,
        22,
        8,
        8,
        Color(
            255,
            90,
            90
        )
    );
}
