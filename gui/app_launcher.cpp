#include "app_launcher.h"

void AppLauncher::render(
    Renderer& renderer
) {

    /* PANEL */

    renderer.drawRoundedRect(
        340,
        110,
        600,
        470,
        28,
        Color(
            18,
            22,
            38,
            235
        )
    );

    /* SEARCH BAR */

    renderer.drawRoundedRect(
        390,
        140,
        500,
        44,
        14,
        Color(
            36,
            42,
            66,
            255
        )
    );

    /* APPS GRID */

    int startX = 390;
    int startY = 220;

    int x = startX;
    int y = startY;

    for (
        int i = 0;
        i < 12;
        i++
    ) {

        renderer.drawRoundedRect(
            x,
            y,
            90,
            90,
            22,
            Color(
                60,
                110,
                255,
                220
            )
        );

        renderer.drawRoundedRect(
            x + 18,
            y + 18,
            54,
            54,
            16,
            Color(
                220,
                235,
                255,
                120
            )
        );

        x += 120;

        if ((i + 1) % 4 == 0) {

            x = startX;
            y += 120;
        }
    }
}
