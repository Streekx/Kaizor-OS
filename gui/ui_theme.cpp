#include "ui_theme.hpp"

// =========================
// WALLPAPER
// =========================
Color UITheme::wallpaper() {
    return Color(8, 12, 20);
}

// =========================
// WINDOW COLORS
// =========================
Color UITheme::focusedWindow() {
    return Color(34, 40, 72, 245);
}

Color UITheme::normalWindow() {
    return Color(28, 32, 58, 235);
}

// =========================
// TITLEBAR
// =========================
Color UITheme::titlebar() {
    return Color(18, 22, 42, 255);
}

// =========================
// TASKBAR
// =========================
Color UITheme::taskbar() {
    return Color(12, 16, 28, 235);
}

// =========================
// DOCK
// =========================
Color UITheme::dock() {
    return Color(20, 24, 42, 220);
}

// =========================
// ACCENT COLORS
// =========================
Color UITheme::accentBlue() {
    return Color(90, 140, 255);
}

Color UITheme::accentPurple() {
    return Color(170, 120, 255);
}

Color UITheme::accentCyan() {
    return Color(80, 220, 255);
}

// =========================
// SHADOW
// =========================
Color UITheme::shadow() {
    return Color(0, 0, 0, 120);
}
