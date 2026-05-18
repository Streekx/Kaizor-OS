#include "blur_engine.hpp"

void BlurEngine::render(
    Renderer& renderer
) {

    /* FAKE GLASS OVERLAY */

    renderer.drawRect(
        0,
        0,
        1280,
        720,
        Color(
            255,
            255,
            255,
            6
        )
    );

    /* DARK GLASS */

    renderer.drawRect(
        0,
        0,
        1280,
        720,
        Color(
            0,
            0,
            0,
            12
        )
    );
}
