#include "cursor.h"

#include "../graphics/renderer.hpp"
#include "../graphics/color.hpp"

void Cursor::render(
    Renderer& renderer,
    int x,
    int y
) {

    renderer.drawRoundedRect(
        x,
        y,
        16,
        22,
        6,
        Color(
            255,
            255,
            255,
            255
        )
    );

    renderer.drawRoundedRect(
        x + 3,
        y + 3,
        10,
        16,
        4,
        Color(
            40,
            40,
            40,
            255
        )
    );
}
