#pragma once
#include <QString>
#include <QtGlobal>
#include "AppColors.h"

namespace AppTheme {

// ═════════════════════════════════════════════════════════════════════════════
//  LAYER 1 — TOKENS
// ═════════════════════════════════════════════════════════════════════════════

constexpr int FontScale = 0;   // change this one value → entire app scales

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

constexpr int GridButtonHeight = 80;   // grid selection button height

inline const QString& fontFamily() {
    static const QString f = QStringLiteral("\"Segoe UI\", system-ui, -apple-system, sans-serif");
    return f;
}

// ═════════════════════════════════════════════════════════════════════════════
//  LAYER 2 — BUILDERS
// ═════════════════════════════════════════════════════════════════════════════

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

inline QString safeExtras(QString e) {
    e = e.trimmed();
    if (!e.isEmpty() && !e.endsWith(';')) e += ';';
    return e;
}

// wrap: guarantees props end with semicolon before wrapping in selector
inline QString wrap(const QString& selector, QString props) {
    return QString("%1 { %2 }").arg(selector, safeExtras(props));
}

// textStyle — properties only (no selector).
// IMPORTANT: background + border are explicit here to prevent cascade bleed.
// When a parent container has QWidget { border: 1px solid X }, Qt cascades
// that rule to all child QWidgets. Explicit overrides here block that bleed.
inline QString textStyle(int px, const QColor& color,
                          int weight = 400, QString extras = {})
{
    return QString("font-size: %1px; font-weight: %2; color: %3; "
                   "background: transparent; border: none; %4")
        .arg(px).arg(weight).arg(rgb(color)).arg(safeExtras(extras));
}

// boxStyle — container with bg + border + radius.
// Uses "QWidget { }" selector so plain QWidget paints its background.
// Children are NOT affected: their own textStyle() overrides the cascade.
inline QString boxStyle(const QColor& bg, const QColor& border,
                         int radius, const QString& selector = "QWidget")
{
    return wrap(selector,
        QString("background: %1; border: 1px solid %2; border-radius: %3px;")
            .arg(rgba(bg)).arg(rgba(border)).arg(radius));
}

// buttonStyle — gradient fill button
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

// outlineButtonStyle — transparent / ghost / outline button
inline QString outlineButtonStyle(const QColor& borderColor, const QColor& textColor,
                                   int fs, int minH,
                                   const QString& hoverBg = {},
                                   const QString& borderStyle = "solid",
                                   int radius = RadiusL)
{
    const QString hover = hoverBg.isEmpty() ? rgba(AppColors::TextPrimary, 13) : hoverBg;
    return QString(R"(
        QPushButton {
            background: transparent;
            border: 1px %1 %2; border-radius: %3px;
            color: %4; font-size: %5px;
            padding: %6px %7px; min-height: %8px;
        }
        QPushButton:hover  { background: %9; }
        QPushButton:disabled { color: %10; border-color: %11; }
    )")
    .arg(borderStyle).arg(rgba(borderColor)).arg(radius)
    .arg(rgb(textColor)).arg(fs)
    .arg(SpaceM).arg(SpaceXXL).arg(minH)
    .arg(hover)
    .arg(rgba(borderColor, borderColor.alpha() / 2))
    .arg(rgba(borderColor, borderColor.alpha() / 2));
}

// radialButtonStyle — grid selection button (all 3 states share this builder)
// cx/cy   = gradient center  (normal: 0.2/0.2,  selected: 0.5/0.0)
// radius  = gradient radius  (normal: 1.5,       selected: 1.2)
// fx/fy   = focal point      (normal: 0.0/0.0,   selected: 0.5/0.0)
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
    .arg(cx, 0, 'f', 1).arg(cy, 0, 'f', 1)
    .arg(gradRadius, 0, 'f', 1)
    .arg(fx, 0, 'f', 1).arg(fy, 0, 'f', 1)
    .arg(rgba(stop0)).arg(rgba(stop1))
    .arg(borderWidth).arg(rgba(borderColor))
    .arg(RadiusL).arg(rgb(AppColors::TextPrimary))
    .arg(GridButtonHeight);
}

// ═════════════════════════════════════════════════════════════════════════════
//  GLOBAL STYLE
// ═════════════════════════════════════════════════════════════════════════════

inline const QString& globalStyle() {
    static const QString s = QString(R"(
        QWidget {
            font-family: %1; font-size: %2px; color: %3;
            background: transparent; border: none;
        }
        QLabel { background: transparent; border: none; }
        QScrollBar:vertical   { background: %4; width: 6px;  border-radius: 3px; }
        QScrollBar:horizontal { background: %4; height: 6px; border-radius: 3px; }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal
            { background: %5; border-radius: 3px; }
        QScrollBar::handle:vertical:hover,
        QScrollBar::handle:horizontal:hover { background: %6; }
        QScrollBar::handle:vertical   { min-height: 20px; }
        QScrollBar::handle:horizontal { min-width:  20px; }
        QScrollBar::add-line:vertical,  QScrollBar::sub-line:vertical  { height: 0px; }
        QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal { width: 0px;  }
    )")
    .arg(fontFamily()).arg(FontBody)
    .arg(rgb(AppColors::TextPrimary))
    .arg(rgba(AppColors::ScrollbarTrack()))
    .arg(rgba(AppColors::ScrollbarThumb()))
    .arg(rgba(AppColors::ScrollbarHover()));
    return s;
}

// ═════════════════════════════════════════════════════════════════════════════
//  LAYER 3 — SEMANTIC STYLES  (static-cached, return const QString&)
// ═════════════════════════════════════════════════════════════════════════════

// ── Labels ────────────────────────────────────────────────────────────────────
inline const QString& labelPrimary()      { static const QString s = textStyle(FontBody,     AppColors::TextPrimary);               return s; }
inline const QString& labelSecondary()    { static const QString s = textStyle(FontBody,     AppColors::TextSecondary);             return s; }
inline const QString& labelMuted()        { static const QString s = textStyle(FontSmall,    AppColors::TextMuted());               return s; }
inline const QString& labelSubtle()       { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle());              return s; }
inline const QString& labelAccentCyan()   { static const QString s = textStyle(FontSmall,    AppColors::Accent());                  return s; }
inline const QString& labelAccentOrange() { static const QString s = textStyle(FontSmall,    AppColors::Primary);                   return s; }

// ── Headings ──────────────────────────────────────────────────────────────────
inline const QString& sectionTitle()      { static const QString s = textStyle(FontSection,  AppColors::TextPrimary,  600);         return s; }
inline const QString& sectionHint()       { static const QString s = textStyle(FontSmall,    AppColors::TextMuted());               return s; }
inline const QString& pageTitle()         { static const QString s = textStyle(FontTitle,    AppColors::TextPrimary,  600);         return s; }
inline const QString& heroTitle()         { static const QString s = textStyle(FontHero,     AppColors::TextPrimary,  600);         return s; }
inline const QString& headerAppTitle()    { static const QString s = textStyle(FontHeader,   AppColors::TextPrimary,  700, "letter-spacing: 2px;"); return s; }
inline const QString& headerAppSubtitle() { static const QString s = textStyle(FontTiny,     AppColors::TextSecondary);             return s; }
inline const QString& laneLabel()         { static const QString s = textStyle(FontBody,     AppColors::TextPrimary);               return s; }
inline const QString& laneSublabel()      { static const QString s = textStyle(FontSmall,    AppColors::TextSecondary);             return s; }
inline const QString& stepLabel()         { static const QString s = textStyle(FontSmall,    AppColors::Primary, 600, "letter-spacing: 2px;"); return s; }

// ── Device ────────────────────────────────────────────────────────────────────
inline const QString& deviceName()    { static const QString s = textStyle(FontBody,  AppColors::TextPrimary,  600); return s; }
inline const QString& deviceAddress() { static const QString s = textStyle(FontTiny,  AppColors::TextSubtle());      return s; }
inline const QString& connectedIcon() {
    static const QString s = wrap("QLabel",
        textStyle(FontSmall, AppColors::Accent(), 700,
                  QString("padding: 0 %1px;").arg(SpaceXS)));
    return s;
}

// ── Summary rows ──────────────────────────────────────────────────────────────
inline const QString& summaryRowLabel()     { static const QString s = textStyle(FontTiny, AppColors::TextMuted(),  400, "letter-spacing: 1px;"); return s; }
inline const QString& summaryRowValue()     { static const QString s = textStyle(FontBody, AppColors::TextPrimary);   return s; }
inline const QString& summaryRowHighlight() { static const QString s = textStyle(FontBody, AppColors::Primary);        return s; }

// ── Help panel ────────────────────────────────────────────────────────────────
inline const QString& helpTitle() { static const QString s = textStyle(FontSubtitle, AppColors::TextPrimary,   600); return s; }
inline const QString& helpItem()  { static const QString s = textStyle(FontSmall,    AppColors::TextSecondary);       return s; }
inline const QString& helpEmail() { static const QString s = textStyle(FontSmall,    AppColors::Accent());             return s; }

// ── Misc labels ───────────────────────────────────────────────────────────────
inline const QString& snackBarText()    { static const QString s = textStyle(FontSubtitle, AppColors::TextPrimary);   return s; }
inline const QString& warningIcon()     { static const QString s = textStyle(FontTitle,    AppColors::Primary);        return s; }
inline const QString& loaderDots()      { static const QString s = textStyle(FontSection,  AppColors::AccentGlow(),  400, "letter-spacing: 6px;"); return s; }
inline const QString& loaderText()      { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle(),  400, "margin-top: 6px;");     return s; }
inline const QString& emptyIcon()       { static const QString s = textStyle(FontTitle,    AppColors::withAlpha(AppColors::TextSubtle(), 120));     return s; }
inline const QString& emptyText()       { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle(),  400, "margin-top: 6px;");     return s; }
inline const QString& scoreValue()      { static const QString s = wrap("QLabel", textStyle(FontHero,     AppColors::TextPrimary, 700)); return s; }
inline const QString& gradeLabel()      { static const QString s = wrap("QLabel", textStyle(FontHero,     AppColors::Primary,     600)); return s; }
inline const QString& instructionStep() { static const QString s = wrap("QLabel", textStyle(FontBody,     AppColors::TextPrimary, 700)); return s; }
inline const QString& instructionItem() { static const QString s = wrap("QLabel", textStyle(FontSmall,    AppColors::withAlpha(AppColors::TextPrimary, 170))); return s; }
inline const QString& gridHeaderCell()  { static const QString s = wrap("QLabel", textStyle(FontTiny,     AppColors::withAlpha(AppColors::TextPrimary, 140), 700, "letter-spacing: 1px;")); return s; }
inline const QString& gridDataCell()    { static const QString s = wrap("QLabel", textStyle(FontBody,     AppColors::TextPrimary)); return s; }
inline const QString& qrScanLabel()     { static const QString s = textStyle(FontTiny, AppColors::TextSecondary, 400, "letter-spacing: 2px;"); return s; }
inline const QString& trainingPlaceholderTitle() { static const QString s = textStyle(FontSection,    AppColors::Accent(),    600); return s; }
inline const QString& trainingPlaceholderBody()  { static const QString s = textStyle(FontSubtitle,   AppColors::TextMuted());      return s; }
inline const QString& trainingPlaceholderIcon()  { static const QString s = textStyle(FontHero + 10,  AppColors::withAlpha(AppColors::Accent(), 120)); return s; }
inline const QString& customButtonMain() { static const QString s = textStyle(FontBody,  AppColors::TextPrimary,  600); return s; }
inline const QString& customButtonSub()  { static const QString s = textStyle(FontTiny,  AppColors::TextMuted());        return s; }
inline const QString& pillLabelActive()  { static const QString s = textStyle(FontSmall, AppColors::PrimaryLight()); return s; }
inline const QString& pillLabelInactive(){ static const QString s = textStyle(FontSmall, AppColors::TextSecondary);   return s; }
inline const QString& cameraView() {
    static const QString s = QString("QLabel { background: %1; %2 }")
        .arg(rgb(AppColors::CameraBg()))
        .arg(textStyle(FontSubtitle, AppColors::withAlpha(AppColors::TextPrimary, 120)));
    return s;
}

// ── Badge ─────────────────────────────────────────────────────────────────────
inline const QString& badge() {
    static const QString s = QString("%1 padding: %2px %3px; border-radius: %4px; border: 1px solid %5;")
        .arg(textStyle(FontTiny, AppColors::TextMuted()))
        .arg(SpaceXS).arg(SpaceM).arg(RadiusPill)
        .arg(rgba(AppColors::BorderMedium()));
    return s;
}
inline const QString& badgeLive() {
    static const QString s = QString(
        "font-size: %1px; font-weight: 600; color: %2; background: %3; "
        "border: 1px solid %4; border-radius: %5px; padding: %6px %7px;")
        .arg(FontTiny).arg(rgb(AppColors::Accent()))
        .arg(rgba(AppColors::Accent(), 20)).arg(rgba(AppColors::Accent(), 100))
        .arg(RadiusPill).arg(SpaceXS).arg(SpaceM);
    return s;
}

// ── Buttons ───────────────────────────────────────────────────────────────────
inline const QString& buttonPrimary() {
    static const QString s = buttonStyle(
        AppColors::Primary, AppColors::PrimaryDark(),
        AppColors::Primary, AppColors::TextOnPrimary(),
        FontBody, 52, RadiusL,
        QString("background: %1; border: 1px solid %2; color: %3;")
            .arg(rgba(AppColors::Primary, 60))
            .arg(rgba(AppColors::Primary, 40))
            .arg(rgba(AppColors::TextOnPrimary(), 100)));
    return s;
}
inline const QString& buttonGhost() {
    static const QString s = outlineButtonStyle(
        AppColors::BorderDashed(), AppColors::TextSecondary,
        FontBody, 52, {}, "dashed");
    return s;
}
inline const QString& buttonDanger() {
    static const QString s = buttonStyle(
        AppColors::Error, AppColors::ErrorDark(),
        AppColors::Error, AppColors::TextPrimary,
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

// Grid selection buttons — all 3 states use radialButtonStyle()
inline const QString& buttonNormal() {
    static const QString s = radialButtonStyle(
        AppColors::withAlpha(AppColors::TextPrimary, 15),
        AppColors::ButtonBgNormal(),
        AppColors::BorderMedium());
    return s;
}
inline const QString& buttonHover() {
    static const QString s = radialButtonStyle(
        AppColors::withAlpha(AppColors::TextPrimary, 25),
        AppColors::ButtonBgNormal(),
        AppColors::withAlpha(AppColors::TextPrimary, 50));
    return s;
}
inline const QString& buttonSelected() {
    // selected: top-center radial (cx:0.5, cy:0), tighter radius, 2px border
    static const QString s = radialButtonStyle(
        AppColors::AccentBg(),
        AppColors::ButtonBgNormal(),
        AppColors::Accent(), 2,
        0.5, 0.0, 1.2, 0.5, 0.0);
    return s;
}

// ── Containers ────────────────────────────────────────────────────────────────
inline const QString& consoleContainer() { static const QString s = boxStyle(AppColors::Surface,                                   AppColors::Border(),       RadiusXXL); return s; }
inline const QString& panel()            { static const QString s = boxStyle(AppColors::SurfaceDeep(),                             AppColors::Border(),       RadiusXL);  return s; }
inline const QString& offlinePanel()     { return panel(); }
inline const QString& deviceCard()       { static const QString s = boxStyle(AppColors::SurfaceCard(),                             AppColors::BorderStrong(), RadiusM);   return s; }
inline const QString& summaryBox()       { static const QString s = boxStyle(AppColors::withAlpha(AppColors::Surface, 242),        AppColors::Border(),       RadiusM);   return s; }
inline const QString& connectedCard() {
    static const QString s = QString("QWidget { background: %1; border: 1px solid %2; border-radius: %3px; }")
        .arg(rgba(AppColors::SurfaceCard())).arg(rgb(AppColors::Accent())).arg(RadiusM);
    return s;
}
// helpPanel — margin keeps it off the side-panel edge (same visual breathing as scanPanel area)
inline const QString& helpPanel() {
    static const QString s = QString("QWidget { background: %1; border: 1px solid %2; "
                                     "border-radius: %3px; margin: 0 %4px %4px %4px; }")
        .arg(rgba(AppColors::withAlpha(AppColors::SurfaceDeep(), 153)))
        .arg(rgba(AppColors::BorderStrong()))
        .arg(RadiusM).arg(SpaceS);
    return s;
}
inline const QString& headerBar() {
    static const QString s = QString(
        "QWidget { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 %1, stop:1 %2); "
        "border: none; border-bottom: 1px solid %3; }")
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
inline const QString& tablePanel()    { static const QString s = QString("QWidget { background: %1; border: 1px solid %2; border-radius: 0px; }").arg(rgb(AppColors::SurfaceDeep())).arg(rgba(AppColors::Border())); return s; }
inline const QString& columnDivider() { static const QString s = QString("QWidget { background: %1; border: none; }").arg(rgba(AppColors::Border())); return s; }
inline const QString& gridHeader()    { static const QString s = QString("QWidget { background: %1; border-bottom: 1px solid %2; }").arg(rgba(AppColors::TextPrimary, 18)).arg(rgba(AppColors::TextPrimary, 25)); return s; }
inline const QString& gridDataRow()   { static const QString s = QString("QWidget { background: %1; border-bottom: 1px solid %2; } QWidget:hover { background: %3; }").arg(rgba(AppColors::TextPrimary, 8)).arg(rgba(AppColors::TextPrimary, 12)).arg(rgba(AppColors::TextPrimary, 14)); return s; }
inline const QString& scorePanel()    { static const QString s = QString("QWidget { background: %1; border-top: 1px solid %2; }").arg(rgba(AppColors::Primary, 40)).arg(rgba(AppColors::Primary, 50)); return s; }

// ── Lightbox / Album ──────────────────────────────────────────────────────────
inline const QString& lightboxBg()    { static const QString s = QString("QWidget { background: %1; border: none; }").arg(rgba(AppColors::Background, 245)); return s; }
inline const QString& captionOverlay(){ static const QString s = QString("QWidget { background: %1; border: none; border-radius: %2px; }").arg(rgba(AppColors::SurfaceOverlay())).arg(RadiusS); return s; }
inline const QString& navArrowButton(){
    static const QString s = QString(
        "QPushButton { background: %1; border: none; border-radius: %2px; "
        "color: %3; font-size: %4px; font-weight: 700; } "
        "QPushButton:hover   { background: %5; } "
        "QPushButton:pressed { background: %6; }")
        .arg(rgb(AppColors::Primary)).arg(RadiusM)
        .arg(rgb(AppColors::TextOnPrimary())).arg(FontHero)
        .arg(rgb(AppColors::PrimaryLight()))
        .arg(rgb(AppColors::PrimaryDark()));
    return s;
}

// ── Misc containers ───────────────────────────────────────────────────────────
inline const QString& trainingPlaceholderBox() {
    static const QString s = QString("background: %1; border: 2px dashed %2; border-radius: %3px;")
        .arg(rgba(AppColors::SurfaceDeep(), 200))
        .arg(rgba(AppColors::Accent(), 80)).arg(RadiusXL);
    return s;
}
inline const QString& qrScanZone() {
    static const QString s = QString(
        "background: qradialgradient(cx:0.5,cy:0.5,radius:0.8,fx:0.5,fy:0.5, "
        "stop:0 %1, stop:1 %2); border: 1px dashed %3; border-radius: %4px;")
        .arg(rgba(AppColors::TextPrimary, 13))
        .arg(rgba(AppColors::Background,  242))
        .arg(rgba(AppColors::TextPrimary,  46))
        .arg(RadiusL);
    return s;
}

// ── Pill ──────────────────────────────────────────────────────────────────────
inline const QString& pillActive()      { static const QString s = QString("PillWidget { background: %1; border: 1px solid %2; border-radius: %3px; }").arg(rgba(AppColors::PrimaryBg())).arg(rgb(AppColors::Primary)).arg(RadiusPill); return s; }
inline const QString& pillInactive()    { static const QString s = QString("PillWidget { background: transparent; border: 1px solid %1; border-radius: %2px; }").arg(rgba(AppColors::DividerColor())).arg(RadiusPill); return s; }
inline const QString& pillDotActive()   { static const QString s = QString("background: %1; border-radius: 4px; border: none;").arg(rgb(AppColors::Primary));        return s; }
inline const QString& pillDotInactive() { static const QString s = QString("background: %1; border-radius: 4px; border: none;").arg(rgb(AppColors::PillInactive())); return s; }

// ── SnackBar — QColor parameter (type-safe) ───────────────────────────────────
inline QString snackBar(const QColor& borderColor) {
    return QString("QWidget { background: %1; border: 1px solid %2; border-radius: %3px; }")
        .arg(rgb(AppColors::Surface)).arg(rgba(borderColor)).arg(RadiusL);
}

} // namespace AppTheme