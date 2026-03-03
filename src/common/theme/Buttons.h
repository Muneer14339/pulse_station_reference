#pragma once
#include "Builders.h"

namespace AppTheme {

inline const QString& buttonPrimary() {
    static const QString s = buttonStyle(
        AppColors::Primary(), AppColors::PrimaryDark(),
        AppColors::Primary(), AppColors::TextOnPrimary(),
        FontBody, 52, RadiusL,
        QString("background: %1; border: 1px solid %2; color: %3;")
            .arg(rgba(AppColors::Primary(), 60))
            .arg(rgba(AppColors::Primary(), 40))
            .arg(rgba(AppColors::TextOnPrimary(), 100)));
    return s;
}

inline const QString& buttonGhost() {
    static const QString s = outlineButtonStyle(
        AppColors::BorderDashed(), AppColors::TextSecondary(),
        FontBody, 52, {}, "dashed");
    return s;
}

inline const QString& buttonDanger() {
    static const QString s = buttonStyle(
        AppColors::Error(), AppColors::ErrorDark(),
        AppColors::Error(), AppColors::TextPrimary(),
        FontSmall, 44);
    return s;
}

inline const QString& refreshButton() {
    static const QString s = outlineButtonStyle(
        AppColors::AccentBorder(), AppColors::Accent(),
        FontBody, 36, rgba(AppColors::Accent(), 26));
    return s;
}

inline const QString& connectButton() {
    static const QString s = outlineButtonStyle(
        AppColors::Accent(), AppColors::Accent(),
        FontSmall, 36, rgba(AppColors::Accent(), 26));
    return s;
}

inline const QString& disconnectButton() {
    static const QString s = buttonStyle(
        AppColors::Accent(), AppColors::AccentHover(),
        AppColors::Accent(), AppColors::TextOnAccent(),
        FontSmall, 36, RadiusM);
    return s;
}

// Grid selection — 3 states, 1 builder
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
