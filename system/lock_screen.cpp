#include "lock_screen.h"

void LockScreen::render(
    Renderer& renderer
) {

    renderer.drawRect(
        0,
        0,
        1280,
        720,
        Color(
            0,
            0,
            0,
            140
        )
    );

    renderer.drawRoundedRect(
        470,
        180,
        340,
        260,
        28,
        Color(
            20,
            24,
            40,
            235
        )
    );

    renderer.drawRoundedRect(
        570,
        220,
        140,
        140,
        70,
        Color(
            90,
            140,
            255,
            255
        )
    );
}
