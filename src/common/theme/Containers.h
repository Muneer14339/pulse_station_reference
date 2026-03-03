#pragma once
#include "Builders.h"

// ─────────────────────────────────────────────────────────────────────────────
//  LEVEL 3: All container styles are CSS properties only — no "QWidget { }"
//  wrapper. Call via widget->setStyleSheet(AppTheme::panel()) and the style
//  applies to that widget only. Children are unaffected.
//
//  Exception: gridDataRow() needs a :hover pseudo-state, which requires a
//  selector. That one keeps "QWidget { } QWidget:hover { }".
// ─────────────────────────────────────────────────────────────────────────────

namespace AppTheme {

// ── Main containers ───────────────────────────────────────────────────────────
inline const QString& consoleContainer() { static const QString s = boxStyle(AppColors::Surface(),                                  AppColors::Border(),       RadiusXXL); return s; }
inline const QString& panel()            { static const QString s = boxStyle(AppColors::SurfaceDeep(),                              AppColors::Border(),       RadiusXL);  return s; }
inline const QString& offlinePanel()     { return panel(); }
inline const QString& deviceCard()       { static const QString s = boxStyle(AppColors::SurfaceCard(),                              AppColors::BorderStrong(), RadiusM);   return s; }
inline const QString& summaryBox()       { static const QString s = boxStyle(AppColors::withAlpha(AppColors::Surface(),  242),      AppColors::Border(),       RadiusM);   return s; }
inline const QString& helpPanel()        { static const QString s = boxStyle(AppColors::withAlpha(AppColors::SurfaceDeep(), 153),   AppColors::BorderStrong(), RadiusM);   return s; }

inline const QString& connectedCard() {
    static const QString s = QString("background: %1; border: 1px solid %2; border-radius: %3px;")
        .arg(rgba(AppColors::SurfaceCard())).arg(rgb(AppColors::Accent())).arg(RadiusM);
    return s;
}
inline const QString& headerBar() {
    static const QString s = QString(
        "background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 %1, stop:1 %2); "
        "border: none; border-bottom: 1px solid %3;")
        .arg(rgba(AppColors::HeaderBg()))
        .arg(rgba(AppColors::HeaderBgAlt()))
        .arg(rgba(AppColors::HeaderBorder()));
    return s;
}
inline const QString& sidePanel() {
    static const QString s = QString("background: transparent; border-right: 1px solid %1;")
        .arg(rgba(AppColors::BorderStrong()));
    return s;
}
inline const QString& scrollArea()  { static const QString s = QStringLiteral("QScrollArea { border: none; background: transparent; }"); return s; }
inline const QString& transparent() { static const QString s = QStringLiteral("background: transparent; border: none;"); return s; }
inline const QString& divider()     { static const QString s = QString("background: %1; height: 1px; border: none;").arg(rgba(AppColors::DividerColor())); return s; }

// ── Table ─────────────────────────────────────────────────────────────────────
inline const QString& tablePanel() {
    static const QString s = QString("background: %1; border: 1px solid %2; border-radius: 0px;")
        .arg(rgb(AppColors::SurfaceDeep())).arg(rgba(AppColors::Border()));
    return s;
}
inline const QString& columnDivider() {
    static const QString s = QString("background: %1; border: none;").arg(rgba(AppColors::Border()));
    return s;
}
inline const QString& gridHeader() {
    static const QString s = QString("background: %1; border-bottom: 1px solid %2;")
        .arg(rgba(AppColors::TextPrimary(), 18)).arg(rgba(AppColors::TextPrimary(), 25));
    return s;
}
// gridDataRow: needs :hover pseudo-state → selector required (only exception to Level 3)
inline const QString& gridDataRow() {
    static const QString s = QString(
        "QWidget { background: %1; border-bottom: 1px solid %2; } "
        "QWidget:hover { background: %3; }")
        .arg(rgba(AppColors::TextPrimary(), 8))
        .arg(rgba(AppColors::TextPrimary(), 12))
        .arg(rgba(AppColors::TextPrimary(), 14));
    return s;
}
inline const QString& scorePanel() {
    static const QString s = QString("background: %1; border-top: 1px solid %2;")
        .arg(rgba(AppColors::Primary(), 40)).arg(rgba(AppColors::Primary(), 50));
    return s;
}

// ── Lightbox / Album ──────────────────────────────────────────────────────────
inline const QString& lightboxBg() {
    static const QString s = QString("background: %1; border: none;").arg(rgba(AppColors::Background(), 245));
    return s;
}
inline const QString& captionOverlay() {
    static const QString s = QString("background: %1; border: none; border-radius: %2px;")
        .arg(rgba(AppColors::SurfaceOverlay())).arg(RadiusS);
    return s;
}

// ── Training ──────────────────────────────────────────────────────────────────
inline const QString& trainingPlaceholderBox() {
    static const QString s = QString("background: %1; border: 2px dashed %2; border-radius: %3px;")
        .arg(rgba(AppColors::SurfaceDeep(), 200))
        .arg(rgba(AppColors::Accent(), 80)).arg(RadiusXL);
    return s;
}

// ── QR scan ───────────────────────────────────────────────────────────────────
inline const QString& qrScanZone() {
    static const QString s = QString(
        "background: qradialgradient(cx:0.5,cy:0.5,radius:0.8,fx:0.5,fy:0.5, "
        "stop:0 %1, stop:1 %2); border: 1px dashed %3; border-radius: %4px;")
        .arg(rgba(AppColors::TextPrimary(), 13))
        .arg(rgba(AppColors::Background(),  242))
        .arg(rgba(AppColors::TextPrimary(),  46))
        .arg(RadiusL);
    return s;
}

// ── Pill ──────────────────────────────────────────────────────────────────────
inline const QString& pillActive()      { static const QString s = QString("PillWidget { background: %1; border: 1px solid %2; border-radius: %3px; }").arg(rgba(AppColors::PrimaryBg())).arg(rgb(AppColors::Primary())).arg(RadiusPill);       return s; }
inline const QString& pillInactive()    { static const QString s = QString("PillWidget { background: transparent; border: 1px solid %1; border-radius: %2px; }").arg(rgba(AppColors::DividerColor())).arg(RadiusPill);                          return s; }
inline const QString& pillDotActive()   { static const QString s = QString("background: %1; border-radius: 4px; border: none;").arg(rgb(AppColors::Primary()));        return s; }
inline const QString& pillDotInactive() { static const QString s = QString("background: %1; border-radius: 4px; border: none;").arg(rgb(AppColors::PillInactive())); return s; }

// ── SnackBar — QColor param, not cached (borderColor varies per call) ─────────
inline QString snackBar(const QColor& borderColor) {
    return QString("background: %1; border: 1px solid %2; border-radius: %3px;")
        .arg(rgb(AppColors::Surface())).arg(rgba(borderColor)).arg(RadiusL);
}

} // namespace AppTheme
