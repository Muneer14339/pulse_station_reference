#pragma once
#include "ui/widgets/HourglassWidget.h"
#include <QLabel>
#include <QWidget>

// ─────────────────────────────────────────────────────────────────────────────
//  Icons.h — single source of truth for every glyph in the application.
//
//  Rules:
//    • No widget file hardcodes "\uXXXX" — all glyphs come from here.
//    • No coupling to AppColors/AppTheme/Tokens — pure glyph declarations.
//    • HourglassAnimator is the only non-glyph dependency (pending animation).
//    • All constexpr — zero runtime cost.
// ─────────────────────────────────────────────────────────────────────────────

namespace AppIcons {

// ── Navigation ────────────────────────────────────────────────────────────────
inline constexpr const char* Back       = "\u2190";   // ←  back / exit panel
inline constexpr const char* Close      = "\u2715";   // ✕  close / dismiss
inline constexpr const char* PrevArrow  = "\u2039";   // ‹  album previous
inline constexpr const char* NextArrow  = "\u203A";   // ›  album next

// ── Session control ───────────────────────────────────────────────────────────
inline constexpr const char* PauseLabel  = "\u23F8  Pause";    // ⏸  Pause  (button label)
inline constexpr const char* ResumeLabel = "\u25B6  Resume";   // ▶  Resume (button label)

// ── Status bar ────────────────────────────────────────────────────────────────
inline constexpr const char* WiFiLabel = "\u25CF Wi-Fi";   // ● Wi-Fi
inline constexpr const char* BleLabel  = "\u2B21  BLE";    // ⬡  BLE

// ── Shot grid ─────────────────────────────────────────────────────────────────
inline constexpr const char* Missed  = "\u25CF";   // ●  missed shot indicator
inline constexpr const char* NoValue = "\u2014";   // —  empty / unavailable cell

// ── Pending hourglass — flyweight singleton, 1 render shared across all usages ─
inline QLabel* pendingLabel(QWidget* parent = nullptr) {
    return HourglassAnimator::instance().createLabel(parent);
}

} // namespace AppIcons