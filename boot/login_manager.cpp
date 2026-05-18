#include "login_manager.h"

void LoginManager::render(
    Renderer& renderer
) {

    /* PANEL */

    renderer.drawRoundedRect(
        430,
        160,
        420,
        380,
        28,
        Color(
            18,
            22,
            36,
            235
        )
    );

    /* AVATAR */

    renderer.drawRoundedRect(
        560,
        200,
        160,
        160,
        80,
        Color(
            90,
            140,
            255,
            255
        )
    );

    /* USER FIELD */

    renderer.drawRoundedRect(
        500,
        410,
        280,
        46,
        14,
        Color(
            40,
            48,
            72,
            255
        )
    );

    /* PASSWORD FIELD */

    renderer.drawRoundedRect(
        500,
        470,
        280,
        46,
        14,
        Color(
            40,
            48,
            72,
            255
        )
    );
}
