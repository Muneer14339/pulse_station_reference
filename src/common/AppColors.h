#pragma once
#include <QColor>
#include <QtGlobal>
#include "theme/ThemePalette.h"

// ─────────────────────────────────────────────────────────────────────────────
//  AppColors — delegates the 7 base tokens to the active ThemePalette.
//
//  BREAKING CHANGE from const → inline function:
//  Old:  AppColors::Primary        (const QColor)
//  New:  AppColors::Primary()      (inline QColor, reads activeTheme())
//
//  Widget code update: add () to Surface and Background in MainWindow.cpp.
//  All other call sites (inside AppTheme headers) already use () form.
// ─────────────────────────────────────────────────────────────────────────────

namespace AppColors {

// ① 7 BASE TOKENS — now runtime-switchable via setTheme()
inline QColor Primary()       { return activeTheme().primary;       }
inline QColor Background()    { return activeTheme().background;     }
inline QColor Surface()       { return activeTheme().surface;        }
inline QColor Success()       { return activeTheme().success;        }
inline QColor Error()         { return activeTheme().error;          }
inline QColor TextPrimary()   { return activeTheme().textPrimary;    }
inline QColor TextSecondary() { return activeTheme().textSecondary;  }

// ② UTILITY
inline QColor withAlpha(const QColor& c, int alpha) {
    return QColor(c.red(), c.green(), c.blue(), qBound(0, alpha, 255));
}

// ③ Accent (alias for Success)
inline QColor Accent()       { return Success();            }
inline QColor AccentBg()     { return withAlpha(Success(),  46); }
inline QColor AccentGlow()   { return withAlpha(Success(), 153); }
inline QColor AccentBorder() { return withAlpha(Success(), 128); }
inline QColor AccentHover()  { return QColor(65, 195, 183);     }

// ④ Primary
inline QColor PrimaryDark()   { return QColor(255, 139,  61); }
inline QColor PrimaryLight()  { return QColor(255, 224, 166); }
inline QColor PrimaryBg()     { return withAlpha(Primary(),   31); }
inline QColor PrimaryBorder() { return withAlpha(Primary(),  128); }

// ⑤ Surface
inline QColor SurfaceDeep()    { return QColor(  9,  14,  27); }
inline QColor SurfaceCard()    { return withAlpha(QColor(13, 19, 40), 204); }
inline QColor SurfaceOverlay() { return withAlpha(Background(), 210); }
inline QColor CameraBg()       { return QColor( 10,  10,  14); }

// ⑥ Text
inline QColor TextMuted()     { return QColor(140, 147, 181); }
inline QColor TextSubtle()    { return QColor(113, 120, 164); }
inline QColor TextOnPrimary() { return QColor( 27,  27,  35); }
inline QColor TextOnAccent()  { return QColor( 10,  15,  25); }

// ⑦ Error
inline QColor ErrorDark() { return QColor(192, 57, 43); }

// ⑧ Borders
inline QColor Border()       { return QColor(255, 255, 255,  20); }
inline QColor BorderMedium() { return QColor(255, 255, 255,  36); }
inline QColor BorderStrong() { return QColor(255, 255, 255,  51); }
inline QColor BorderDashed() { return QColor(255, 255, 255,  64); }

// ⑨ Header
inline QColor HeaderBg()     { return withAlpha(QColor( 9, 13, 30), 242); }
inline QColor HeaderBgAlt()  { return withAlpha(QColor(13, 19, 40), 242); }
inline QColor HeaderBorder() { return QColor(255, 255, 255, 15); }

// ⑩ Misc
inline QColor DividerColor()   { return QColor(255, 255, 255,  31); }
inline QColor ScrollbarTrack() { return QColor(255, 255, 255,   5); }
inline QColor ScrollbarThumb() { return QColor(255, 255, 255,  38); }
inline QColor ScrollbarHover() { return QColor(255, 255, 255,  64); }
inline QColor PillInactive()   { return QColor( 96, 103, 138); }
inline QColor ButtonBgNormal() { return withAlpha(QColor(7, 9, 20), 230); }

// ⑪ Status
inline QColor StatusSuccess() { return withAlpha(Success(), 180); }
inline QColor StatusError()   { return withAlpha(Error(),   180); }
inline QColor StatusInfo()    { return QColor(255, 255, 255, 36); }

} // namespace AppColors
