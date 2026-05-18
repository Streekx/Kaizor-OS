#include "desktop_icons.hpp"

void DesktopIcons::render(
    Renderer& renderer
) {

    int x = 48;
    int y = 90;

    for (
        int i = 0;
        i < 4;
        i++
    ) {

        renderer.drawRoundedRect(
            x,
            y,
            72,
            72,
            18,
            Color(
                40 + (i * 20),
                120,
                255,
                220
            )
        );

        renderer.drawRoundedRect(
            x + 14,
            y + 14,
            44,
            44,
            12,
            Color(
                220,
                240,
                255,
                140
            )
        );

        y += 108;
    }
}
