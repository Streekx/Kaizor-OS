#pragma once
#include "../graphics/color.hpp"

struct GradientPair {
    Color top;
    Color bottom;
};

class UITheme {
public:
    // ─── Base surfaces ─────────────────────────────────────────
    static Color base()              { return Color( 8, 10, 20); }
    static Color surface()           { return Color(18, 22, 40, 240); }
    static Color surfaceRaised()     { return Color(26, 32, 56, 245); }
    static Color surfaceHigh()       { return Color(36, 44, 78, 250); }

    // ─── Window ────────────────────────────────────────────────
    static Color windowFocused()     { return Color(24, 30, 58, 248); }
    static Color windowNormal()      { return Color(18, 24, 46, 235); }
    static Color titlebarFocused()   { return Color(20, 26, 52, 255); }
    static Color titlebarNormal()    { return Color(15, 20, 40, 255); }

    static GradientPair titlebarGrad()  { return {Color(28, 35, 65, 255), Color(18, 24, 50, 255)}; }
    static GradientPair windowGrad()    { return {Color(22, 28, 54, 252), Color(16, 22, 44, 252)}; }

    // ─── Text ──────────────────────────────────────────────────
    static Color textPrimary()       { return Color(235, 240, 255); }
    static Color textSecondary()     { return Color(160, 175, 210); }
    static Color textMuted()         { return Color(100, 115, 155); }
    static Color textAccent()        { return Color(130, 180, 255); }
    static Color textDanger()        { return Color(255, 100, 100); }
    static Color textSuccess()       { return Color( 80, 220, 130); }

    // ─── Accent palette ────────────────────────────────────────
    static Color accentBlue()        { return Color( 80, 140, 255); }
    static Color accentPurple()      { return Color(160, 110, 255); }
    static Color accentCyan()        { return Color( 70, 210, 245); }
    static Color accentGreen()       { return Color( 70, 200, 120); }
    static Color accentOrange()      { return Color(255, 150,  60); }
    static Color accentRed()         { return Color(255,  80,  80); }
    static Color accentPink()        { return Color(255, 100, 180); }

    // ─── Taskbar ───────────────────────────────────────────────
    static Color taskbar()           { return Color(10, 14, 28, 230); }
    static GradientPair taskbarGrad(){ return {Color(16, 20, 38, 235), Color( 8, 12, 26, 235)}; }
    static Color taskbarSearch()     { return Color(30, 38, 68, 200); }
    static Color taskbarSeparator()  { return Color(50, 60, 100, 100); }

    // ─── Dock ──────────────────────────────────────────────────
    static Color dock()              { return Color(16, 20, 40, 220); }
    static Color dockBorder()        { return Color(60, 75, 130, 100); }
    static Color dockIcon()          { return Color(45, 60, 110, 230); }
    static Color dockIconHover()     { return Color(70, 95, 175, 245); }
    static Color dockIconActive()    { return Color(55, 80, 150, 240); }

    // ─── Input ─────────────────────────────────────────────────
    static Color inputBg()           { return Color(25, 32, 60, 220); }
    static Color inputBorder()       { return Color(55, 70, 120, 180); }
    static Color inputBorderFocus()  { return Color( 80, 140, 255, 220); }

    // ─── Buttons ───────────────────────────────────────────────
    static Color buttonPrimary()     { return Color( 65, 120, 245); }
    static Color buttonPrimaryHov()  { return Color( 90, 145, 255); }
    static Color buttonSecondary()   { return Color( 35, 44, 80, 220); }
    static Color buttonSecondaryHov(){ return Color( 50, 62, 110, 230); }

    // ─── Scrollbars ────────────────────────────────────────────
    static Color scrollbarBg()       { return Color(20, 26, 50, 160); }
    static Color scrollbarThumb()    { return Color(60, 80, 140, 200); }

    // ─── Shadows ───────────────────────────────────────────────
    static Color shadowDeep()        { return Color(0, 0, 0, 160); }
    static Color shadowMid()         { return Color(0, 0, 0, 100); }
    static Color shadowLight()       { return Color(0, 0, 0,  55); }
    static Color shadow()            { return Color(0, 0, 0, 110); }

    // ─── Traffic lights ────────────────────────────────────────
    static Color btnClose()          { return Color(255,  95,  86); }
    static Color btnMinimize()       { return Color(255, 189,  46); }
    static Color btnMaximize()       { return Color( 39, 201,  63); }

    // ─── Borders / separators ──────────────────────────────────
    static Color borderSubtle()      { return Color(45, 55, 95, 120); }
    static Color borderMid()         { return Color(60, 75, 130, 160); }
    static Color borderBright()      { return Color(80, 100, 170, 200); }

    // ─── Wallpaper blobs ───────────────────────────────────────
    static Color blobBlue()          { return Color( 70, 110, 255,  20); }
    static Color blobPurple()        { return Color(160,  80, 255,  16); }
    static Color blobCyan()          { return Color( 70, 220, 210,  16); }

    // ─── Desktop icons ─────────────────────────────────────────
    static Color desktopIconBg()     { return Color(40, 55, 110, 215); }
    static Color desktopIconGlow()   { return Color(90, 140, 255,  40); }

    // Legacy aliases kept for compatibility
    static Color wallpaper()         { return base(); }
    static Color focusedWindow()     { return windowFocused(); }
    static Color normalWindow()      { return windowNormal(); }
    static Color titlebar()          { return titlebarFocused(); }
};
