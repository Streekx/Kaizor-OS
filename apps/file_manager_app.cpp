#include "file_manager_app.hpp"

void FileManagerApp::render(
    Renderer& renderer
) {

    /* SIDEBAR */

    renderer.drawRoundedRect(
        180,
        110,
        220,
        520,
        20,
        Color(
            26,
            32,
            54,
            230
        )
    );

    int y = 150;

    for (
        int i = 0;
        i < 7;
        i++
    ) {

        renderer.drawRoundedRect(
            210,
            y,
            150,
            38,
            12,
            Color(
                44,
                56,
                92,
                220
            )
        );

        y += 52;
    }

    /* MAIN AREA */

    renderer.drawRoundedRect(
        430,
        110,
        640,
        520,
        20,
        Color(
            34,
            40,
            72,
            220
        )
    );

    int startX = 470;
    int startY = 160;

    int x = startX;
    int fileY = startY;

    for (
        int i = 0;
        i < 12;
        i++
    ) {

        renderer.drawRoundedRect(
            x,
            fileY,
            90,
            90,
            18,
            Color(
                80,
                140,
                255,
                220
            )
        );

        x += 130;

        if ((i + 1) % 4 == 0) {

            x = startX;
            fileY += 120;
        }
    }
}
