#include "settings_app.hpp"

void SettingsApp::render(
    Renderer& renderer
) {

    /* SIDEBAR */

    renderer.drawRoundedRect(
        260,
        150,
        220,
        420,
        18,
        Color(
            26,
            30,
            52,
            230
        )
    );

    int y = 190;

    for (
        int i = 0;
        i < 6;
        i++
    ) {

        renderer.drawRoundedRect(
            285,
            y,
            160,
            42,
            12,
            Color(
                50,
                64,
                110,
                220
            )
        );

        y += 58;
    }

    /* CONTENT */

    renderer.drawRoundedRect(
        510,
        150,
        500,
        420,
        18,
        Color(
            34,
            40,
            70,
            220
        )
    );

    /* TOGGLES */

    int toggleY = 210;

    for (
        int i = 0;
        i < 5;
        i++
    ) {

        renderer.drawRoundedRect(
            560,
            toggleY,
            380,
            56,
            16,
            Color(
                44,
                52,
                88,
                255
            )
        );

        renderer.drawRoundedRect(
            860,
            toggleY + 12,
            56,
            28,
            14,
            Color(
                90,
                180,
                255,
                255
            )
        );

        toggleY += 72;
    }
}
