#include "blur_engine.hpp"

BlurEngine::BlurEngine() {
}

void BlurEngine::render(
    Renderer& renderer
) {

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
