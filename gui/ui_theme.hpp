#pragma once

#include "../graphics/color.hpp"

class UITheme {

public:

    /* =========================
       WALLPAPER
       ========================= */

    static Color wallpaper() {

        return Color(
            8,
            12,
            20
        );
    }

    /* =========================
       WINDOW COLORS
       ========================= */

    static Color focusedWindow() {

        return Color(
            34,
            40,
            72,
            245
        );
    }

    static Color normalWindow() {

        return Color(
            28,
            32,
            58,
            235
        );
    }

    /* =========================
       TITLEBAR
       ========================= */

    static Color titlebar() {

        return Color(
            18,
            22,
            42,
            255
        );
    }

    /* =========================
       TASKBAR
       ========================= */

    static Color taskbar() {

        return Color(
            12,
            16,
            28,
            235
        );
    }

    /* =========================
       DOCK
       ========================= */

    static Color dock() {

        return Color(
            20,
            24,
            42,
            220
        );
    }

    /* =========================
       ACCENT
       ========================= */

    static Color accentBlue() {

        return Color(
            90,
            140,
            255
        );
    }

    static Color accentPurple() {

        return Color(
            170,
            120,
            255
        );
    }

    static Color accentCyan() {

        return Color(
            80,
            220,
            255
        );
    }

    /* =========================
       SHADOW
       ========================= */

    static Color shadow() {

        return Color(
            0,
            0,
            0,
            120
        );
    }
};
