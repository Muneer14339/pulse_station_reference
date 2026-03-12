#pragma once
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QLabel>

// ─────────────────────────────────────────────────────────────────────────────
//  HourglassAnimator — Flyweight singleton.
//
//  ONE render per tick, ONE pixmap in memory.
//  Any number of QLabels can display it — zero extra CPU/memory per label.
//
//  Usage (via Icons.h):
//    QLabel* lbl = AppIcons::pendingLabel(parentWidget);
//    // Done. Label auto-updates every frame via signal.
// ─────────────────────────────────────────────────────────────────────────────

class HourglassAnimator : public QObject {
    Q_OBJECT
public:
    static HourglassAnimator& instance();

    // Returns a QLabel pre-connected to frameReady — caller owns the label
    QLabel* createLabel(QWidget* parent = nullptr);

signals:
    void frameReady(const QPixmap& px);

private:
    explicit HourglassAnimator(QObject* parent = nullptr);
    void renderFrame();

    QTimer* m_ringTimer;
    QTimer* m_sandTimer;
    int     m_ringAngle = 0;
    int     m_sandFrame = 0;

    static constexpr int Size = 44;
};