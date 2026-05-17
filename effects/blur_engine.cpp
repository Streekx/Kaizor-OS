#include "blur_engine.hpp"

void BlurEngine::renderBlurLayer(
    Renderer& renderer,
    int x,
    int y,
    int width,
    int height
) {

    renderer.drawRect(
        x,
        y,
        width,
        height,
        Color(
            255,
            255,
            255,
            30
        )
    );
}
