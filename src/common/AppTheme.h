#pragma once
#include <QString>
#include "AppColors.h"

namespace AppTheme {
  
        inline QString getGlobalStyle() {
    return QString(R"(
        QWidget {
            font-family: "Segoe UI", system-ui, -apple-system, sans-serif;
            color: rgb(245, 245, 255);
            background: transparent;
            border: none;
        }
        QLabel {
            background: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background: rgba(255, 255, 255, 5);
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 38);
            border-radius: 3px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 64);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar:horizontal {
            background: rgba(255, 255, 255, 5);
            height: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:horizontal {
            background: rgba(255, 255, 255, 38);
            border-radius: 3px;
            min-width: 20px;
        }
        QScrollBar::handle:horizontal:hover {
            background: rgba(255, 255, 255, 64);
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )");
}


    inline QString getConsoleBorder() {
        return QString("border: 1px solid rgba(255, 255, 255, 20); border-radius: 22px;");
    }
    
    inline QString getPanelStyle() {
        return QString("background: rgba(9, 14, 27, 255); border: 1px solid rgba(255, 255, 255, 20); border-radius: 16px;");
    }
    
    inline QString getButtonStyle() {
        return QString(R"(
            QPushButton {
                background: qradialgradient(cx: 0.2, cy: 0.2, radius: 1.5,
                    fx: 0, fy: 0,
                    stop: 0 rgba(255, 255, 255, 15),
                    stop: 1 rgba(7, 9, 20, 230));
                border: 1px solid rgba(255, 255, 255, 36);
                border-radius: 12px;
                color: rgb(245, 245, 255);
                text-align: left;
                padding: 0px;
                min-height: 60px;
            }
            QLabel {
                border: none;
                background: transparent;
            }
        )");
    }
    
    inline QString getButtonHoverStyle() {
        return QString(R"(
            QPushButton {
                background: qradialgradient(cx: 0.2, cy: 0.2, radius: 1.5,
                    fx: 0, fy: 0,
                    stop: 0 rgba(255, 255, 255, 25),
                    stop: 1 rgba(7, 9, 20, 230));
                border: 1px solid rgba(255, 255, 255, 50);
                border-radius: 12px;
                color: rgb(245, 245, 255);
                text-align: left;
                padding: 0px;
                min-height: 60px;
            }
            QLabel {
                border: none;
                background: transparent;
            }
        )");
    }
    
    inline QString getButtonSelectedStyle() {
        return QString(R"(
            QPushButton {
                background: qradialgradient(cx: 0.5, cy: 0, radius: 1.2,
                    fx: 0.5, fy: 0,
                    stop: 0 rgba(79, 209, 197, 46),
                    stop: 1 rgba(7, 11, 24, 242));
                border: 2px solid rgba(79, 209, 197, 255);
                border-radius: 12px;
                color: rgb(245, 245, 255);
                text-align: left;
                padding: 0px;
                min-height: 60px;
            }
            QLabel {
                border: none;
                background: transparent;
            }
        )");
    }
    
    inline QString getButtonPrimaryStyle() {
        return QString(R"(
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop: 0 rgb(255, 182, 73),
                stop: 1 rgb(255, 139, 61));
            border: 1px solid rgb(255, 182, 73);
            border-radius: 12px;
            color: rgb(27, 27, 35);
            font-weight: 600;
            text-align: center;
            padding: 12px;
            min-height: 40px;
        )");
    }
    
    inline QString getDividerStyle() {
        return QString("background: rgba(255, 255, 255, 31); height: 1px; border: none;");
    }


}