#pragma once
#include "Builders.h"

namespace AppTheme {

inline const QString& buttonPrimary() {
    static const QString s = buttonStyle(
        AppColors::Primary(), AppColors::PrimaryDark(),
        AppColors::Primary(), AppColors::TextOnPrimary(),
        FontBody, MinBtnHeight, RadiusL,
        QString("background: %1; border: 1px solid %2; color: %3;")
            .arg(rgba(AppColors::Primary(), 60))
            .arg(rgba(AppColors::Primary(), 40))
            .arg(rgba(AppColors::TextOnPrimary(), 100)));
    return s;
}

inline const QString& buttonGhost() {
    static const QString s = outlineButtonStyle(
        AppColors::BorderDashed(), AppColors::TextSecondary(),
        FontBody, MinBtnHeight, {}, "dashed");
    return s;
}

inline const QString& buttonDanger() {
    static const QString s = buttonStyle(
        AppColors::Error(), AppColors::ErrorDark(),
        AppColors::Error(), AppColors::TextPrimary(),
        FontSmall, MinBtnHeight);
    return s;
}

// ── Icon-only square button (pair with setFixedSize(IconBtnSz, IconBtnSz)) ────
// Zero padding so the icon glyph is never clipped inside the fixed-size box.
inline const QString& iconButton() {
    static const QString s = QString(R"(
        QPushButton {
            background: transparent;
            border: 1px solid %1;
            border-radius: %2px;
            color: %3;
            font-size: %4px;
            font-weight: 600;
            padding: 0px;
            min-height: 0px;
        }
        QPushButton:hover   { background: %5; }
        QPushButton:pressed { background: %6; }
        QPushButton:disabled { color: %7; border-color: %8; }
    )")
    .arg(rgba(AppColors::AccentBorder()))
    .arg(RadiusM)
    .arg(rgb(AppColors::Accent()))
    .arg(FontBody)
    .arg(rgba(AppColors::Accent(), 26))
    .arg(rgba(AppColors::Accent(), 51))
    .arg(rgba(AppColors::Accent(), 80))
    .arg(rgba(AppColors::AccentBorder(), 80));
    return s;
}

// ── Connect / Disconnect ──────────────────────────────────────────────────────
inline const QString& connectButton() {
    static const QString s = outlineButtonStyle(
        AppColors::Accent(), AppColors::Accent(),
        FontSmall, MinBtnHeight, rgba(AppColors::Accent(), 26));
    return s;
}

inline const QString& disconnectButton() {
    static const QString s = buttonStyle(
        AppColors::Accent(), AppColors::AccentHover(),
        AppColors::Accent(), AppColors::TextOnAccent(),
        FontSmall, MinBtnHeight, RadiusM);
    return s;
}

// ── Tab bar buttons ───────────────────────────────────────────────────────────
// Active: accent underline + tinted bg. Inactive: muted text, hover reveals bg.
inline const QString& tabActive() {
    static const QString s = QString(R"(
        QPushButton {
            background: %1;
            border: none;
            border-bottom: 2px solid %2;
            border-radius: 0px;
            color: %3;
            font-size: %4px;
            font-weight: 600;
            padding: %5px %6px;
            min-height: %7px;
        }
    )")
    .arg(rgba(AppColors::Accent(), 20))
    .arg(rgb(AppColors::Accent()))
    .arg(rgb(AppColors::Accent()))
    .arg(FontBody)
    .arg(SpaceM).arg(SpaceXXL)
    .arg(TabHeight);
    return s;
}

inline const QString& tabInactive() {
    static const QString s = QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            border-bottom: 2px solid transparent;
            border-radius: 0px;
            color: %1;
            font-size: %2px;
            padding: %3px %4px;
            min-height: %5px;
        }
        QPushButton:hover {
            background: %6;
            color: %7;
            border-bottom: 2px solid %8;
        }
    )")
    .arg(rgb(AppColors::TextSecondary()))
    .arg(FontBody)
    .arg(SpaceM).arg(SpaceXXL)
    .arg(TabHeight)
    .arg(rgba(AppColors::TextPrimary(), 12))
    .arg(rgb(AppColors::TextPrimary()))
    .arg(rgba(AppColors::TextPrimary(), 40));
    return s;
}

// ── Grid selection — 3 states ─────────────────────────────────────────────────
inline const QString& buttonNormal() {
    static const QString s = radialButtonStyle(
        AppColors::withAlpha(AppColors::TextPrimary(), 15),
        AppColors::ButtonBgNormal(),
        AppColors::BorderMedium());
    return s;
}

inline const QString& buttonHover() {
    static const QString s = radialButtonStyle(
        AppColors::withAlpha(AppColors::TextPrimary(), 25),
        AppColors::ButtonBgNormal(),
        AppColors::withAlpha(AppColors::TextPrimary(), 50));
    return s;
}

inline const QString& buttonSelected() {
    static const QString s = radialButtonStyle(
        AppColors::AccentBg(),
        AppColors::ButtonBgNormal(),
        AppColors::Accent(), 2,
        0.5, 0.0, 1.2, 0.5, 0.0);
    return s;
}

inline const QString& navArrowButton() {
    static const QString s = QString(
        "QPushButton { background: %1; border: none; border-radius: %2px; "
        "color: %3; font-size: %4px; font-weight: 700; } "
        "QPushButton:hover   { background: %5; } "
        "QPushButton:pressed { background: %6; }")
        .arg(rgb(AppColors::Primary())).arg(RadiusM)
        .arg(rgb(AppColors::TextOnPrimary())).arg(FontHero)
        .arg(rgb(AppColors::PrimaryLight()))
        .arg(rgb(AppColors::PrimaryDark()));
    return s;
}

} // namespace AppTheme