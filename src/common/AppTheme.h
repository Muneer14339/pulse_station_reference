#pragma once
#include <QString>
#include "AppColors.h"

/**
 * @brief Centralized style sheet provider for PulseStation.
 *
 * RULES:
 *  - Never write raw setStyleSheet() strings in widget code.
 *  - Every visual style must come from one of these functions.
 *  - Font sizes are tuned for a wall-mounted display used by 40+ age users.
 *  - To change a color / font globally, edit ONLY this file.
 */
namespace AppTheme {

// ─────────────────────────────────────────────────────────────────────────────
//  Font Size Constants  (wall-mount display, 40+ users — keep these large)
// ─────────────────────────────────────────────────────────────────────────────
constexpr int FontTiny      = 13;   // device addresses, letter-spaced labels
constexpr int FontSmall     = 15;   // hints, help text, badge text
constexpr int FontBody      = 17;   // standard body / list items
constexpr int FontSubtitle  = 19;   // section subtitles, card sub-labels
constexpr int FontSection   = 21;   // section headings (Step 1, Step 2 …)
constexpr int FontTitle     = 26;   // page / screen titles
constexpr int FontHero      = 34;   // full-screen hero titles
constexpr int FontHeader    = 24;   // top-bar application title

// ─────────────────────────────────────────────────────────────────────────────
//  Global Application Style
// ─────────────────────────────────────────────────────────────────────────────
inline QString globalStyle() {
    return QStringLiteral(R"(
        QWidget {
            font-family: "Segoe UI", system-ui, -apple-system, sans-serif;
            font-size: 17px;
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
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
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
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Label Styles
// ─────────────────────────────────────────────────────────────────────────────
inline QString labelPrimary() {
    return QStringLiteral("font-size: 17px; color: rgb(245, 245, 255); background: transparent; border: none;");
}
inline QString labelSecondary() {
    return QStringLiteral("font-size: 17px; color: rgb(179, 185, 214); background: transparent; border: none;");
}
inline QString labelMuted() {
    return QStringLiteral("font-size: 15px; color: rgb(140, 147, 181); background: transparent; border: none;");
}
inline QString labelSubtle() {
    return QStringLiteral("font-size: 15px; color: rgb(113, 120, 164); background: transparent; border: none;");
}
inline QString labelAccentCyan() {
    return QStringLiteral("font-size: 15px; color: rgb(79, 209, 197); background: transparent; border: none;");
}
inline QString labelAccentOrange() {
    return QStringLiteral("font-size: 15px; color: rgb(255, 182, 73); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Section & Page Title Styles
// ─────────────────────────────────────────────────────────────────────────────
inline QString sectionTitle() {
    return QStringLiteral("font-size: 19px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
}
inline QString sectionHint() {
    return QStringLiteral("font-size: 15px; color: rgb(140, 147, 181); background: transparent; border: none;");
}
inline QString pageTitle() {
    return QStringLiteral("font-size: 26px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
}
inline QString heroTitle() {
    return QStringLiteral("font-size: 34px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
}
inline QString headerAppTitle() {
    return QStringLiteral("font-size: 22px; letter-spacing: 2px; font-weight: bold; color: rgb(245, 245, 255); background: transparent; border: none;");
}
inline QString headerAppSubtitle() {
    return QStringLiteral("font-size: 14px; color: rgb(179, 185, 214); background: transparent; border: none;");
}
inline QString laneLabel() {
    return QStringLiteral("font-size: 17px; color: rgb(199, 204, 240); background: transparent; border: none;");
}
inline QString laneSublabel() {
    return QStringLiteral("font-size: 15px; color: rgb(179, 185, 214); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Step Flow
// ─────────────────────────────────────────────────────────────────────────────
inline QString stepLabel() {
    return QStringLiteral("font-size: 15px; color: rgb(255, 182, 73); letter-spacing: 2px; font-weight: 600; background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Badge
// ─────────────────────────────────────────────────────────────────────────────
inline QString badge() {
    return QStringLiteral("font-size: 14px; color: rgb(192, 197, 227); "
                          "padding: 3px 10px; border-radius: 999px; "
                          "border: 1px solid rgba(255, 255, 255, 36); "
                          "background: transparent;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Custom Button (grid selection buttons)
// ─────────────────────────────────────────────────────────────────────────────
inline QString customButtonMain() {
    return QStringLiteral("font-weight: 600; font-size: 17px; color: rgb(245, 245, 255); background: transparent; border: none;");
}
inline QString customButtonSub() {
    return QStringLiteral("font-size: 14px; color: rgb(165, 172, 201); background: transparent; border: none;");
}
inline QString buttonNormal() {
    return QStringLiteral(R"(
        QPushButton {
            background: qradialgradient(cx:0.2, cy:0.2, radius:1.5, fx:0, fy:0,
                stop:0 rgba(255,255,255,15), stop:1 rgba(7,9,20,230));
            border: 1px solid rgba(255, 255, 255, 36);
            border-radius: 12px;
            color: rgb(245, 245, 255);
            text-align: left;
            padding: 0px;
            min-height: 70px;
        }
        QLabel { border: none; background: transparent; }
    )");
}
inline QString buttonHover() {
    return QStringLiteral(R"(
        QPushButton {
            background: qradialgradient(cx:0.2, cy:0.2, radius:1.5, fx:0, fy:0,
                stop:0 rgba(255,255,255,25), stop:1 rgba(7,9,20,230));
            border: 1px solid rgba(255, 255, 255, 50);
            border-radius: 12px;
            color: rgb(245, 245, 255);
            text-align: left;
            padding: 0px;
            min-height: 70px;
        }
        QLabel { border: none; background: transparent; }
    )");
}
inline QString buttonSelected() {
    return QStringLiteral(R"(
        QPushButton {
            background: qradialgradient(cx:0.5, cy:0, radius:1.2, fx:0.5, fy:0,
                stop:0 rgba(79,209,197,46), stop:1 rgba(7,11,24,242));
            border: 2px solid rgba(79, 209, 197, 255);
            border-radius: 12px;
            color: rgb(245, 245, 255);
            text-align: left;
            padding: 0px;
            min-height: 70px;
        }
        QLabel { border: none; background: transparent; }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Primary Action Button  (orange gradient — confirm / next)
// ─────────────────────────────────────────────────────────────────────────────
inline QString buttonPrimary() {
    return QStringLiteral(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgb(255,182,73), stop:1 rgb(255,139,61));
            border: 1px solid rgb(255, 182, 73);
            border-radius: 12px;
            color: rgb(27, 27, 35);
            font-weight: 600;
            font-size: 17px;
            padding: 14px 24px;
            min-height: 48px;
        }
        QPushButton:disabled {
            background: rgba(255, 182, 73, 60);
            border: 1px solid rgba(255, 182, 73, 40);
            color: rgba(27, 27, 35, 100);
        }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Secondary / Ghost Action Buttons  (back, reset)
// ─────────────────────────────────────────────────────────────────────────────
inline QString buttonGhost() {
    return QStringLiteral(R"(
        QPushButton {
            background: transparent;
            border: 1px dashed rgba(255, 255, 255, 64);
            border-radius: 12px;
            color: rgb(179, 185, 214);
            padding: 14px 24px;
            min-height: 48px;
            font-size: 17px;
        }
        QPushButton:hover { background: rgba(255, 255, 255, 13); }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Bluetooth Panel Buttons
// ─────────────────────────────────────────────────────────────────────────────
inline QString refreshButton() {
    return QStringLiteral(R"(
        QPushButton {
            background: transparent;
            border: 1px solid rgba(79, 209, 197, 128);
            border-radius: 6px;
            color: rgb(79, 209, 197);
            padding: 4px 8px;
            font-size: 18px;
        }
        QPushButton:hover { background: rgba(79, 209, 197, 26); }
        QPushButton:disabled {
            color: rgba(79, 209, 197, 60);
            border-color: rgba(79, 209, 197, 40);
        }
    )");
}
inline QString connectButton() {
    return QStringLiteral(R"(
        QPushButton {
            background: transparent;
            border: 1px solid rgb(79, 209, 197);
            border-radius: 8px;
            color: rgb(79, 209, 197);
            padding: 8px 18px;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover { background: rgba(79, 209, 197, 26); }
    )");
}
inline QString disconnectButton() {
    return QStringLiteral(R"(
        QPushButton {
            background: rgb(79, 209, 197);
            border: none;
            border-radius: 8px;
            color: rgb(10, 15, 25);
            padding: 8px 18px;
            font-size: 15px;
            font-weight: 600;
        }
        QPushButton:hover { background: rgb(65, 195, 183); }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Containers / Panels
// ─────────────────────────────────────────────────────────────────────────────
inline QString consoleContainer() {
    return QStringLiteral("background: rgb(17, 23, 41); "
                          "border: 1px solid rgba(255, 255, 255, 20); "
                          "border-radius: 22px;");
}
inline QString headerBar() {
    return QStringLiteral("QWidget { "
                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
                          "  stop:0 rgba(9,13,30,242), stop:1 rgba(13,19,40,242)); "
                          "border: none; "
                          "border-bottom: 1px solid rgba(255, 255, 255, 15); }");
}
inline QString sidePanel() {
    return QStringLiteral("background: transparent; "
                          "border-right: 1px solid rgba(255, 255, 255, 51);");
}
inline QString panel() {
    return QStringLiteral("background: rgba(9, 14, 27, 255); "
                          "border: 1px solid rgba(255, 255, 255, 20); "
                          "border-radius: 16px;");
}
inline QString offlinePanel() {
    return panel(); // same visual, different semantic purpose
}
inline QString helpPanel() {
    return QStringLiteral("background: rgba(9, 14, 27, 153); "
                          "border: 1px solid rgba(255, 255, 255, 51); "
                          "border-radius: 10px;");
}
inline QString deviceCard() {
    return QStringLiteral("QWidget { "
                          "background: rgba(13, 19, 40, 204); "
                          "border: 1px solid rgba(255, 255, 255, 51); "
                          "border-radius: 10px; }");
}
inline QString connectedCard() {
    return QStringLiteral("QWidget { "
                          "background: rgba(13, 19, 40, 204); "
                          "border: 1px solid rgb(79, 209, 197); "
                          "border-radius: 10px; }");
}
inline QString scrollArea() {
    return QStringLiteral("QScrollArea { border: none; background: transparent; }");
}
inline QString transparent() {
    return QStringLiteral("background: transparent; border: none;");
}
inline QString summaryBox() {
    return QStringLiteral("background: rgba(12, 18, 32, 242); "
                          "border: 1px solid rgba(255, 255, 255, 26); "
                          "border-radius: 10px;");
}
inline QString divider() {
    return QStringLiteral("background: rgba(255, 255, 255, 31); height: 1px; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Bluetooth – Device Labels
// ─────────────────────────────────────────────────────────────────────────────
inline QString deviceName() {
    return QStringLiteral("font-size: 17px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
}
inline QString deviceAddress() {
    return QStringLiteral("font-size: 13px; color: rgb(113, 120, 164); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Loading / Empty States
// ─────────────────────────────────────────────────────────────────────────────
inline QString loaderDots() {
    return QStringLiteral("font-size: 20px; color: rgba(79,209,197,160); letter-spacing: 6px; background: transparent; border: none;");
}
inline QString loaderText() {
    return QStringLiteral("font-size: 15px; color: rgb(113,120,164); background: transparent; border: none; margin-top: 6px;");
}
inline QString emptyIcon() {
    return QStringLiteral("font-size: 30px; color: rgba(113,120,164,120); background: transparent; border: none;");
}
inline QString emptyText() {
    return QStringLiteral("font-size: 15px; color: rgb(113,120,164); background: transparent; border: none; margin-top: 6px;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Summary Box Rows
// ─────────────────────────────────────────────────────────────────────────────
inline QString summaryRowLabel() {
    return QStringLiteral("font-size: 13px; color: rgb(157, 164, 197); letter-spacing: 1px; background: transparent; border: none;");
}
inline QString summaryRowValue() {
    return QStringLiteral("font-size: 17px; color: rgb(212, 216, 243); background: transparent; border: none;");
}
inline QString summaryRowHighlight() {
    return QStringLiteral("font-size: 17px; color: rgb(255, 182, 73); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Help Panel Labels
// ─────────────────────────────────────────────────────────────────────────────
inline QString helpTitle() {
    return QStringLiteral("font-size: 16px; font-weight: 600; color: rgb(199, 204, 240); background: transparent; border: none;");
}
inline QString helpItem() {
    return QStringLiteral("font-size: 15px; color: rgb(165, 172, 201); background: transparent; border: none;");
}
inline QString helpEmail() {
    return QStringLiteral("font-size: 15px; color: rgb(79, 209, 197); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  SnackBar
// ─────────────────────────────────────────────────────────────────────────────
inline QString snackBar(const QString& borderColor) {
    return QString("QWidget { background: rgb(9, 13, 30); border: 1px solid %1; border-radius: 12px; }")
           .arg(borderColor);
}
inline QString snackBarText() {
    return QStringLiteral("font-size: 16px; color: rgb(230, 233, 255); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  QR Panel
// ─────────────────────────────────────────────────────────────────────────────
inline QString qrScanZone() {
    return QStringLiteral("background: qradialgradient(cx:0.5, cy:0.5, radius:0.8, fx:0.5, fy:0.5, "
                          "  stop:0 rgba(255,255,255,13), stop:1 rgba(5,7,16,242)); "
                          "border: 1px dashed rgba(255, 255, 255, 46); "
                          "border-radius: 12px;");
}
inline QString qrScanLabel() {
    return QStringLiteral("font-size: 14px; color: rgb(163, 173, 212); letter-spacing: 2px;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Pill Widget States
// ─────────────────────────────────────────────────────────────────────────────
inline QString pillActive() {
    return QStringLiteral("PillWidget { background: rgba(255,182,73,31); "
                          "border: 1px solid rgb(255,182,73); border-radius: 999px; }");
}
inline QString pillInactive() {
    return QStringLiteral("PillWidget { background: transparent; "
                          "border: 1px solid rgba(255,255,255,31); border-radius: 999px; }");
}
inline QString pillDotActive() {
    return QStringLiteral("background: rgb(255,182,73); border-radius: 4px; border: none;");
}
inline QString pillDotInactive() {
    return QStringLiteral("background: rgb(96,103,138); border-radius: 4px; border: none;");
}
inline QString pillLabelActive() {
    return QStringLiteral("font-size: 15px; color: rgb(255,224,166); background: transparent; border: none;");
}
inline QString pillLabelInactive() {
    return QStringLiteral("font-size: 15px; color: rgb(179,185,214); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Icon Labels
// ─────────────────────────────────────────────────────────────────────────────
inline QString warningIcon() {
    return QStringLiteral("font-size: 26px; color: rgb(255,182,73); background: transparent; border: none;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Status Badges
// ─────────────────────────────────────────────────────────────────────────────
inline QString badgeLive() {
    return QStringLiteral("font-size: 14px; font-weight: 600; color: rgb(79,209,197); "
                          "background: rgba(79,209,197,20); "
                          "border: 1px solid rgba(79,209,197,100); "
                          "border-radius: 999px; padding: 4px 12px;");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Training Placeholder
// ─────────────────────────────────────────────────────────────────────────────
inline QString trainingPlaceholderBox() {
    return QStringLiteral("background: rgba(9, 14, 27, 200); "
                          "border: 2px dashed rgba(79, 209, 197, 80); "
                          "border-radius: 16px;");
}
inline QString trainingPlaceholderIcon() {
    return QStringLiteral("font-size: 48px; color: rgba(79,209,197,120); background: transparent; border: none;");
}
inline QString trainingPlaceholderTitle() {
    return QStringLiteral("font-size: 22px; font-weight: 600; color: rgb(79, 209, 197); background: transparent; border: none;");
}
inline QString trainingPlaceholderBody() {
    return QStringLiteral("font-size: 16px; color: rgb(140, 147, 181); background: transparent; border: none;");
}

// ──────────────────────────────────────────────────────────────────────────────
// AppTheme.h  ─  ADD these methods to the existing class
// ──────────────────────────────────────────────────────────────────────────────

// Red/danger button  (Stop, Pause)
static inline QString buttonDanger() {
    return QStringLiteral(R"(
        QPushButton {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 rgb(231,76,60), stop:1 rgb(192,57,43));
            border: 1px solid rgb(231,76,60);
            border-radius: 12px;
            color: rgb(255,255,255);
            font-weight: 600;
            font-size: 14px;
            padding: 10px 18px;
            min-height: 40px;
        }
        QPushButton:hover  { background: rgb(231,76,60); }
        QPushButton:pressed{ background: rgb(192,57,43); }
    )");
}

// Left-side camera feed area
static inline QString cameraView() {
    return QStringLiteral(R"(
        QLabel {
            background: rgb(10,10,14);
            color: rgba(255,255,255,120);
            font-size: 16px;
        }
    )");
}

// Connected device icon text  (● Wi-Fi  /  ⬡ BLE)
static inline QString connectedIcon() {
    return QStringLiteral(R"(
        QLabel {
            color: rgb(0,206,201);
            font-size: 14px;
            font-weight: 700;
            background: transparent;
            padding: 0 4px;
        }
    )");
}

// Instruction guide step heading  "1. Start"
static inline QString instructionStep() {
    return QStringLiteral(R"(
        QLabel {
            color: rgb(255,255,255);
            font-size: 13px;
            font-weight: 700;
            background: transparent;
        }
    )");
}

// Instruction bullet item
static inline QString instructionItem() {
    return QStringLiteral(R"(
        QLabel {
            color: rgba(255,255,255,170);
            font-size: 12px;
            background: transparent;
        }
    )");
}

// Shot grid header background row
static inline QString gridHeader() {
    return QStringLiteral(R"(
        QWidget {
            background: rgba(255,255,255,18);
            border-bottom: 1px solid rgba(255,255,255,25);
        }
    )");
}

// Shot grid header cell text
static inline QString gridHeaderCell() {
    return QStringLiteral(R"(
        QLabel {
            color: rgba(255,255,255,140);
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 1px;
            text-transform: uppercase;
            background: transparent;
        }
    )");
}

// Shot grid data row
static inline QString gridDataRow() {
    return QStringLiteral(R"(
        QWidget {
            background: rgba(255,255,255,8);
            border-bottom: 1px solid rgba(255,255,255,12);
        }
        QWidget:hover { background: rgba(255,255,255,14); }
    )");
}

// Shot grid data cell text
static inline QString gridDataCell() {
    return QStringLiteral(R"(
        QLabel {
            color: rgb(255,255,255);
            font-size: 13px;
            background: transparent;
        }
    )");
}

// Total score box
static inline QString scorePanel() {
    return QStringLiteral(R"(
        QWidget {
            background: rgba(139,90,43,0.45);
            border-top: 1px solid rgba(255,182,73,50);
        }
    )");
}

// Big score number
static inline QString scoreValue() {
    return QStringLiteral(R"(
        QLabel {
            color: rgb(255,255,255);
            font-size: 36px;
            font-weight: 700;
            background: transparent;
        }
    )");
}

} // namespace AppTheme
