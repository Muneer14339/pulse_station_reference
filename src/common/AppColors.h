#pragma once
#include <QColor>

/**
 * @brief Centralized color palette for PulseStation.
 *
 * ALL colors used in the application must come from here.
 * Never use raw RGB values in widget code.
 */
namespace AppColors {

    // ── Background ────────────────────────────────────────────────────────
    const QColor Background               = QColor(5,   8,  20);
    const QColor BackgroundGradientTop    = QColor(18,  25,  50);
    const QColor BackgroundGradientBottom = QColor(5,   8,  20);

    const QColor ConsoleBackground        = QColor(17,  23,  41);
    const QColor ConsoleBorder            = QColor(255, 255, 255, 20);

    const QColor HeaderBackground         = QColor(9,  13,  30, 242);
    const QColor HeaderBackgroundAlt      = QColor(13, 19,  40, 242);
    const QColor HeaderBorder             = QColor(255, 255, 255, 15);

    // ── Text ──────────────────────────────────────────────────────────────
    const QColor TextPrimary    = QColor(245, 245, 255);
    const QColor TextSecondary  = QColor(179, 185, 214);
    const QColor TextMuted      = QColor(139, 145, 178);
    const QColor TextHint       = QColor(140, 147, 181);
    const QColor TextSubtle     = QColor(113, 120, 164);

    // ── Accent – Orange ───────────────────────────────────────────────────
    const QColor AccentOrange      = QColor(255, 182,  73);
    const QColor AccentOrangeDark  = QColor(255, 139,  61);
    const QColor AccentOrangeLight = QColor(255, 224, 166);
    const QColor AccentOrangeBg    = QColor(255, 182,  73, 31);

    // ── Accent – Cyan ─────────────────────────────────────────────────────
    const QColor AccentCyan     = QColor( 79, 209, 197);
    const QColor AccentCyanBg   = QColor( 79, 209, 197, 46);
    const QColor AccentCyanGlow = QColor( 79, 209, 197, 153);

    // ── Panels ────────────────────────────────────────────────────────────
    const QColor PanelBackground    = QColor(9,  14,  27);
    const QColor PanelBorder        = QColor(255, 255, 255, 20);
    const QColor PanelBackgroundAlt = QColor(9,  13,  30, 245);
    const QColor PanelCard          = QColor(13,  19,  40, 204);
    const QColor PanelCardBorder    = QColor(255, 255, 255, 51);

    // ── Buttons ───────────────────────────────────────────────────────────
    const QColor ButtonBackground = QColor(7,   9,  20, 230);
    const QColor ButtonBorder     = QColor(255, 255, 255, 36);
    const QColor ButtonHover      = QColor(255, 255, 255, 15);

    // ── Misc ──────────────────────────────────────────────────────────────
    const QColor DividerColor          = QColor(255, 255, 255, 31);
    const QColor ScrollbarTrack        = QColor(255, 255, 255,  5);
    const QColor ScrollbarThumb        = QColor(255, 255, 255, 38);
    const QColor ScrollbarThumbHover   = QColor(255, 255, 255, 64);
    const QColor PillInactive          = QColor( 96, 103, 138);
    const QColor PillBorder            = QColor(255, 255, 255, 31);

    // ── Status ────────────────────────────────────────────────────────────
    const QColor StatusSuccess = QColor( 79, 209, 197, 180);
    const QColor StatusError   = QColor(255, 100, 100, 180);
    const QColor StatusInfo    = QColor(255, 255, 255,  36);
}
