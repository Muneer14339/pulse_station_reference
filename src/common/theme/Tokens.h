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

// ── Spacing tokens ────────────────────────────────────────────────────────────
constexpr int SpaceXS   =  4;
constexpr int SpaceS    =  8;
constexpr int SpaceM    = 12;
constexpr int SpaceL    = 16;
constexpr int SpaceXL   = 20;
constexpr int SpaceXXL  = 24;
constexpr int SpaceXXXL = 32;

// ── Border radius ─────────────────────────────────────────────────────────────
constexpr int RadiusS    =  6;
constexpr int RadiusM    = 10;
constexpr int RadiusL    = 12;
constexpr int RadiusXL   = 16;
constexpr int RadiusXXL  = 22;
constexpr int RadiusPill = 999;

// ── Component sizes ───────────────────────────────────────────────────────────
constexpr int GridButtonHeight = 80;
constexpr int IconBtnSz        = 36;   // standard square icon button (↻ ✕ ← ⬇)
constexpr int TabHeight        = 52;   // tab bar button height
constexpr int MinBtnHeight     = 44;   // minimum tappable button height

// ── Layout — side panels ──────────────────────────────────────────────────────
// SidebarMinW is a MINIMUM only — use setMinimumWidth(), never setFixedWidth().
// 340px guarantees "Bluetooth Connections" + icon button fit at PanelPadH=16 margins.
constexpr int SidebarMinW = 340;
// Keep alias so old references still compile during migration
constexpr int SidebarW    = SidebarMinW;

// ── Layout — padding tokens ───────────────────────────────────────────────────
// Use these in setContentsMargins() — never hardcode pixel values in widget files.
constexpr int PanelPadH  = SpaceL;    // 16 — side panel inner horizontal padding
constexpr int PanelPadV  = SpaceL;    // 16 — side panel inner vertical padding
constexpr int ContentH   = SpaceXXL;  // 24 — main content area horizontal padding
constexpr int ContentV   = SpaceXL;   // 20 — main content area vertical padding
constexpr int CardPadH   = SpaceL;    // 16 — card/summary box horizontal padding
constexpr int CardPadV   = SpaceM;    // 12 — card/summary box vertical padding

// ── Layout — spacing tokens ───────────────────────────────────────────────────
// Use these in setSpacing() — never hardcode pixel values in widget files.
constexpr int SectionGap = SpaceXL;   // 20 — between major sections
constexpr int ItemGap    = SpaceM;    // 12 — between items within a section
constexpr int InlineGap  = SpaceS;    //  8 — between inline/sibling elements
constexpr int RowPad     = SpaceXS;   //  4 — top/bottom padding inside rows

inline const QString& fontFamily() {
    static const QString f = QStringLiteral("\"Segoe UI\", system-ui, -apple-system, sans-serif");
    return f;
}

} // namespace AppTheme