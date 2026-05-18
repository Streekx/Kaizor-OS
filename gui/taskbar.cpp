#include "taskbar.hpp"
#include "ui_theme.hpp"

void Taskbar::render(Renderer& renderer) {

    // Taskbar background
    renderer.drawRect(
        0,
        0,
        1280,
        42,
        UITheme::taskbar()
    );

    // Logo
    renderer.drawRect(
        16,
        10,
        22,
        22,
        UITheme::accentBlue()
    );

    // Status icons
    renderer.drawRect(
        1180,
        12,
        12,
        12,
        UITheme::accentCyan()
    );

    renderer.drawRect(
        1204,
        12,
        12,
        12,
        UITheme::accentPurple()
    );

    renderer.drawRect(
        1228,
        12,
        12,
        12,
        Color(255, 90, 90)
    );
}
