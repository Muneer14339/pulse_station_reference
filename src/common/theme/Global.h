#pragma once
#include <QString>
#include "Builders.h"

namespace AppTheme {

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
        QScrollBar::handle:horizontal { min-width: 20px; }
        QScrollBar::add-line:vertical,  QScrollBar::sub-line:vertical  { height: 0px; }
        QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal { width: 0px; }
    )")
    .arg(fontFamily()).arg(FontBody)
    .arg(rgb(AppColors::TextPrimary()))
    .arg(rgba(AppColors::ScrollbarTrack()))
    .arg(rgba(AppColors::ScrollbarThumb()))
    .arg(rgba(AppColors::ScrollbarHover()));
    return s;
}

} // namespace AppTheme
