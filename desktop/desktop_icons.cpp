#include "desktop_icons.hpp"

void DesktopIcons::render(
    Renderer& renderer
) {

    /* FILES */

    renderer.drawRect(
        50,
        90,
        64,
        64,
        Color(
            90,
            140,
            255
        )
    );

    /* BROWSER */

    renderer.drawRect(
        50,
        190,
        64,
        64,
        Color(
            255,
            120,
            90
        )
    );

    /* TERMINAL */

    renderer.drawRect(
        50,
        290,
        64,
        64,
        Color(
            80,
            255,
            120
        )
    );

    /* SETTINGS */

    renderer.drawRect(
        50,
        390,
        64,
        64,
        Color(
            180,
            120,
            255
        )
    );
}
