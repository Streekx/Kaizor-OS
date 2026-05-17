#include "wallpaper.hpp"

void Wallpaper::render(
    Renderer& renderer
) {

    /* MAIN WALLPAPER */

    renderer.clear(
        Color(
            14,
            18,
            28
        )
    );

    /* GLOW EFFECT */

    renderer.drawRect(
        920,
        120,
        220,
        220,
        Color(
            60,
            90,
            255,
            40
        )
    );

    renderer.drawRect(
        850,
        220,
        160,
        160,
        Color(
            120,
            80,
            255,
            30
        )
    );
}
