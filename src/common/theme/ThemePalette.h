#pragma once
#include <QColor>

// ─────────────────────────────────────────────────────────────────────────────
//  ThemePalette — holds the 7 semantic base tokens for the active theme.
//
//  Usage:
//    setTheme(AppPalettes::light());   // switch to light theme
//    setTheme(AppPalettes::dark());    // switch back to dark
//
//  After setTheme(), re-apply stylesheets:
//    qApp->setStyleSheet(AppTheme::globalStyle());
//    // then re-call setStyleSheet on each widget
//
//  NOTE: Layer-3 static caches are optimized for single-theme embedded apps.
//  For live runtime switching, remove the `static` keyword from cached styles
//  or call AppTheme::refreshAll() after setTheme().
// ─────────────────────────────────────────────────────────────────────────────

struct ThemePalette {
    QColor primary;
    QColor background;
    QColor surface;
    QColor success;
    QColor error;
    QColor textPrimary;
    QColor textSecondary;
};

namespace AppPalettes {

inline ThemePalette dark() {
    return {
        QColor(255, 182,  73),   // primary       — orange
        QColor(  5,   8,  20),   // background     — near black
        QColor( 17,  23,  41),   // surface        — dark navy
        QColor( 79, 209, 197),   // success/accent — cyan
        QColor(255, 100, 100),   // error          — red
        QColor(245, 245, 255),   // textPrimary    — near white
        QColor(179, 185, 214),   // textSecondary  — muted blue-grey
    };
}

inline ThemePalette light() {
    return {
        QColor(210, 130,  20),   // primary       — deeper orange (legible on white)
        QColor(245, 247, 252),   // background     — near white
        QColor(255, 255, 255),   // surface        — white
        QColor( 20, 175, 165),   // success/accent — teal
        QColor(200,  55,  55),   // error          — deep red
        QColor( 15,  18,  35),   // textPrimary    — near black
        QColor( 80,  92, 125),   // textSecondary  — muted dark blue
    };
}

} // namespace AppPalettes

inline ThemePalette& activeTheme() {
    static ThemePalette t = AppPalettes::dark();
    return t;
}

inline void setTheme(const ThemePalette& palette) {
    activeTheme() = palette;
}
