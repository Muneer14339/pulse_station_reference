#pragma once
#include "Builders.h"

namespace AppTheme {

inline const QString& labelPrimary()      { static const QString s = textStyle(FontBody,     AppColors::TextPrimary());               return s; }
inline const QString& labelSecondary()    { static const QString s = textStyle(FontBody,     AppColors::TextSecondary());             return s; }
inline const QString& labelMuted()        { static const QString s = textStyle(FontSmall,    AppColors::TextMuted());                 return s; }
inline const QString& labelSubtle()       { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle());                return s; }
inline const QString& labelAccentCyan()   { static const QString s = textStyle(FontSmall,    AppColors::Accent());                    return s; }
inline const QString& labelAccentOrange() { static const QString s = textStyle(FontSmall,    AppColors::Primary());                   return s; }

inline const QString& sectionTitle()      { static const QString s = textStyle(FontSection,  AppColors::TextPrimary(),  600);         return s; }
inline const QString& sectionHint()       { static const QString s = textStyle(FontSmall,    AppColors::TextMuted());                 return s; }
inline const QString& pageTitle()         { static const QString s = textStyle(FontTitle,    AppColors::TextPrimary(),  600);         return s; }
inline const QString& heroTitle()         { static const QString s = textStyle(FontHero,     AppColors::TextPrimary(),  600);         return s; }
inline const QString& headerAppTitle()    { static const QString s = textStyle(FontHeader,   AppColors::TextPrimary(),  700, "letter-spacing: 2px;"); return s; }
inline const QString& headerAppSubtitle() { static const QString s = textStyle(FontTiny,     AppColors::TextSecondary());             return s; }
inline const QString& laneLabel()         { static const QString s = textStyle(FontBody,     AppColors::TextPrimary());               return s; }
inline const QString& laneSublabel()      { static const QString s = textStyle(FontSmall,    AppColors::TextSecondary());             return s; }
inline const QString& stepLabel()         { static const QString s = textStyle(FontSmall,    AppColors::Primary(), 600, "letter-spacing: 2px;"); return s; }

inline const QString& deviceName()    { static const QString s = textStyle(FontBody,  AppColors::TextPrimary(),  600); return s; }
inline const QString& deviceAddress() { static const QString s = textStyle(FontTiny,  AppColors::TextSubtle());        return s; }
inline const QString& connectedIcon() {
    static const QString s = wrap("QLabel",
        textStyle(FontSmall, AppColors::Accent(), 700,
                  QString("padding: 0 %1px;").arg(SpaceXS)));
    return s;
}

inline const QString& summaryRowLabel()     { static const QString s = textStyle(FontTiny, AppColors::TextMuted(),    400, "letter-spacing: 1px;"); return s; }
inline const QString& summaryRowValue()     { static const QString s = textStyle(FontBody, AppColors::TextPrimary());   return s; }
inline const QString& summaryRowHighlight() { static const QString s = textStyle(FontBody, AppColors::Primary());        return s; }

inline const QString& helpTitle() { static const QString s = textStyle(FontSubtitle, AppColors::TextPrimary(),   600); return s; }
inline const QString& helpItem()  { static const QString s = textStyle(FontSmall,    AppColors::TextSecondary());        return s; }
inline const QString& helpEmail() { static const QString s = textStyle(FontSmall,    AppColors::Accent());               return s; }

inline const QString& snackBarText()    { static const QString s = textStyle(FontSubtitle, AppColors::TextPrimary());    return s; }
inline const QString& warningIcon()     { static const QString s = textStyle(FontTitle,    AppColors::Primary());         return s; }
inline const QString& loaderDots()      { static const QString s = textStyle(FontSection,  AppColors::AccentGlow(),  400, "letter-spacing: 6px;"); return s; }
inline const QString& loaderText()      { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle(),  400, "margin-top: 6px;");     return s; }
inline const QString& emptyIcon()       { static const QString s = textStyle(FontTitle,    AppColors::withAlpha(AppColors::TextSubtle(), 120));     return s; }
inline const QString& emptyText()       { static const QString s = textStyle(FontSmall,    AppColors::TextSubtle(),  400, "margin-top: 6px;");     return s; }
inline const QString& scoreValue()      { static const QString s = wrap("QLabel", textStyle(FontHero,     AppColors::TextPrimary(), 700)); return s; }
inline const QString& gradeLabel()      { static const QString s = wrap("QLabel", textStyle(FontHero,     AppColors::Primary(),     600)); return s; }
inline const QString& instructionStep() { static const QString s = wrap("QLabel", textStyle(FontBody,     AppColors::TextPrimary(), 700)); return s; }
inline const QString& instructionItem() { static const QString s = wrap("QLabel", textStyle(FontSmall,    AppColors::withAlpha(AppColors::TextPrimary(), 170))); return s; }
inline const QString& gridHeaderCell()  { static const QString s = wrap("QLabel", textStyle(FontTiny,     AppColors::withAlpha(AppColors::TextPrimary(), 140), 700, "letter-spacing: 1px;")); return s; }
inline const QString& gridDataCell()    { static const QString s = wrap("QLabel", textStyle(FontBody,     AppColors::TextPrimary())); return s; }
inline const QString& qrScanLabel()     { static const QString s = textStyle(FontTiny, AppColors::TextSecondary(), 400, "letter-spacing: 2px;"); return s; }
inline const QString& trainingPlaceholderTitle() { static const QString s = textStyle(FontSection,   AppColors::Accent(),    600); return s; }
inline const QString& trainingPlaceholderBody()  { static const QString s = textStyle(FontSubtitle,  AppColors::TextMuted());      return s; }
inline const QString& trainingPlaceholderIcon()  { static const QString s = textStyle(FontHero + 10, AppColors::withAlpha(AppColors::Accent(), 120)); return s; }
inline const QString& customButtonMain() { static const QString s = textStyle(FontBody,  AppColors::TextPrimary(),  600); return s; }
inline const QString& customButtonSub()  { static const QString s = textStyle(FontTiny,  AppColors::TextMuted());          return s; }
inline const QString& pillLabelActive()  { static const QString s = textStyle(FontSmall, AppColors::PrimaryLight()); return s; }
inline const QString& pillLabelInactive(){ static const QString s = textStyle(FontSmall, AppColors::TextSecondary());  return s; }
inline const QString& cameraView() {
    static const QString s = QString("QLabel { background: %1; %2 }")
        .arg(rgb(AppColors::CameraBg()))
        .arg(textStyle(FontSubtitle, AppColors::withAlpha(AppColors::TextPrimary(), 120)));
    return s;
}

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

inline const QString& gridMissedCell()   { static const QString s = wrap("QLabel", textStyle(FontBody, AppColors::Error(),     700)); return s; }
inline const QString& gridPendingCell()  { static const QString s = wrap("QLabel", textStyle(FontBody, AppColors::TextMuted()       )); return s; }

} // namespace AppTheme
