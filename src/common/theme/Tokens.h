#pragma once
#include <QString>

namespace AppTheme {

// ── Global font scale — change ONE value → entire app scales ──────────────────
constexpr int FontScale = 0;

constexpr int FontTiny     = 15 + FontScale;
constexpr int FontSmall    = 17 + FontScale;
constexpr int FontBody     = 19 + FontScale;
constexpr int FontSubtitle = 21 + FontScale;
constexpr int FontSection  = 23 + FontScale;
constexpr int FontTitle    = 30 + FontScale;
constexpr int FontHero     = 38 + FontScale;
constexpr int FontHeader   = 26 + FontScale;

constexpr int SpaceXS   =  4;
constexpr int SpaceS    =  8;
constexpr int SpaceM    = 12;
constexpr int SpaceL    = 16;
constexpr int SpaceXL   = 20;
constexpr int SpaceXXL  = 24;
constexpr int SpaceXXXL = 32;

constexpr int RadiusS    =  6;
constexpr int RadiusM    = 10;
constexpr int RadiusL    = 12;
constexpr int RadiusXL   = 16;
constexpr int RadiusXXL  = 22;
constexpr int RadiusPill = 999;

constexpr int GridButtonHeight = 80;

inline const QString& fontFamily() {
    static const QString f = QStringLiteral("\"Segoe UI\", system-ui, -apple-system, sans-serif");
    return f;
}

} // namespace AppTheme
