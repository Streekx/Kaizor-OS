#include "terminal_app.hpp"

void TerminalApp::render(
    Renderer& renderer
) {

    /* TERMINAL BODY */

    renderer.drawRoundedRect(
        220,
        120,
        840,
        500,
        22,
        Color(
            10,
            12,
            18,
            245
        )
    );

    /* TITLEBAR */

    renderer.drawRoundedRect(
        220,
        120,
        840,
        42,
        22,
        Color(
            24,
            28,
            44,
            255
        )
    );

    /* BUTTONS */

    renderer.drawRect(
        240,
        136,
        10,
        10,
        Color(
            255,
            95,
            86
        )
    );

    renderer.drawRect(
        258,
        136,
        10,
        10,
        Color(
            255,
            189,
            46
        )
    );

    renderer.drawRect(
        276,
        136,
        10,
        10,
        Color(
            39,
            201,
            63
        )
    );

    /* TERMINAL LINES */

    int y = 190;

    for (
        int i = 0;
        i < 14;
        i++
    ) {

        renderer.drawRect(
            260,
            y,
            440,
            4,
            Color(
                90,
                255,
                120
            )
        );

        y += 24;
    }
}
