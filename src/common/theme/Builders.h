#pragma once
#include <QString>
#include <QtGlobal>
#include "Tokens.h"
#include "../AppColors.h"

namespace AppTheme {

// ── Color serializers ─────────────────────────────────────────────────────────
inline QString rgb(const QColor& c) {
    return QString("rgb(%1,%2,%3)").arg(c.red()).arg(c.green()).arg(c.blue());
}
inline QString rgba(const QColor& c) {
    return QString("rgba(%1,%2,%3,%4)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}
inline QString rgba(const QColor& c, int alpha) {
    alpha = qBound(0, alpha, 255);
    return QString("rgba(%1,%2,%3,%4)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(alpha);
}

// ── Guards ────────────────────────────────────────────────────────────────────
inline QString safeExtras(QString e) {
    e = e.trimmed();
    if (!e.isEmpty() && !e.endsWith(';')) e += ';';
    return e;
}

inline QString wrap(const QString& selector, QString props) {
    return QString("%1 { %2 }").arg(selector, safeExtras(props));
}

// ── textStyle — CSS properties only, no selector ─────────────────────────────
//  background + border explicit: prevents Qt cascade bleed from parent containers.
inline QString textStyle(int px, const QColor& color,
                          int weight = 400, QString extras = {})
{
    return QString("font-size: %1px; font-weight: %2; color: %3; "
                   "background: transparent; border: none; %4")
        .arg(px).arg(weight).arg(rgb(color)).arg(safeExtras(extras));
}

// ── boxStyle — CSS properties only, no selector ──────────────────────────────
//  Call via widget->setStyleSheet(boxStyle(...)) — applies to that widget only.
//  Children are unaffected: their textStyle() explicitly overrides cascade.
inline QString boxStyle(const QColor& bg, const QColor& border, int radius) {
    return safeExtras(
        QString("background: %1; border: 1px solid %2; border-radius: %3px;")
            .arg(rgba(bg)).arg(rgba(border)).arg(radius));
}

// ── buttonStyle — gradient fill button ───────────────────────────────────────
inline QString buttonStyle(const QColor& bgStart, const QColor& bgEnd,
                            const QColor& borderColor, const QColor& textColor,
                            int fs, int minH, int radius = RadiusL,
                            const QString& disabledProps = {})
{
    QString s = QString(R"(
        QPushButton {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 %1, stop:1 %2);
            border: 1px solid %3; border-radius: %4px;
            color: %5; font-weight: 600; font-size: %6px;
            padding: %7px %8px; min-height: %9px;
        }
    )")
    .arg(rgb(bgStart)).arg(rgb(bgEnd))
    .arg(rgb(borderColor)).arg(radius)
    .arg(rgb(textColor)).arg(fs)
    .arg(SpaceM).arg(SpaceXXL).arg(minH);
    if (!disabledProps.isEmpty())
        s += wrap("QPushButton:disabled", disabledProps);
    return s;
}

// ── outlineButtonStyle — ghost / outline button ───────────────────────────────
inline QString outlineButtonStyle(const QColor& borderColor, const QColor& textColor,
                                   int fs, int minH,
                                   const QString& hoverBg = {},
                                   const QString& borderStyle = "solid",
                                   int radius = RadiusL)
{
    const QString hover = hoverBg.isEmpty() ? rgba(AppColors::TextPrimary(), 13) : hoverBg;
    return QString(R"(
        QPushButton {
            background: transparent;
            border: 1px %1 %2; border-radius: %3px;
            color: %4; font-size: %5px;
            padding: %6px %7px; min-height: %8px;
        }
        QPushButton:hover    { background: %9; }
        QPushButton:disabled { color: %10; border-color: %11; }
    )")
    .arg(borderStyle).arg(rgba(borderColor)).arg(radius)
    .arg(rgb(textColor)).arg(fs)
    .arg(SpaceM).arg(SpaceXXL).arg(minH)
    .arg(hover)
    .arg(rgba(borderColor, borderColor.alpha() / 2))
    .arg(rgba(borderColor, borderColor.alpha() / 2));
}

// ── radialButtonStyle — grid selection button (all 3 states share this) ───────
inline QString radialButtonStyle(const QColor& stop0, const QColor& stop1,
                                  const QColor& borderColor, int borderWidth = 1,
                                  double cx = 0.2, double cy = 0.2,
                                  double gradRadius = 1.5,
                                  double fx = 0.0, double fy = 0.0)
{
    return QString(R"(
        QPushButton {
            background: qradialgradient(cx:%1,cy:%2,radius:%3,fx:%4,fy:%5,
                stop:0 %6, stop:1 %7);
            border: %8px solid %9; border-radius: %10px;
            color: %11; text-align: left; padding: 0px; min-height: %12px;
        }
        QLabel { border: none; background: transparent; }
    )")
    .arg(cx,0,'f',1).arg(cy,0,'f',1)
    .arg(gradRadius,0,'f',1)
    .arg(fx,0,'f',1).arg(fy,0,'f',1)
    .arg(rgba(stop0)).arg(rgba(stop1))
    .arg(borderWidth).arg(rgba(borderColor))
    .arg(RadiusL).arg(rgb(AppColors::TextPrimary()))
    .arg(GridButtonHeight);
}

} // namespace AppTheme
