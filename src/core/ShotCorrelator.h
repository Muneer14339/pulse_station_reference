#pragma once
#include <QObject>
#include <QTimer>
#include "training/data/ReviewDataTypes.h"

// ─────────────────────────────────────────────────────────────────────────────
//  ShotCorrelator — correlates BLE trigger signals with camera detections.
//
//  Three cases:
//   1. BLE → camera within window  : normal shot, splitTime from BLE timestamps
//   2. BLE → window expires        : missed shot, score=0, splitTime from BLE
//   3. Camera → no BLE pending     : camera-only shot, splitTime from arrival time
// ─────────────────────────────────────────────────────────────────────────────

class ShotCorrelator : public QObject {
    Q_OBJECT
public:
    explicit ShotCorrelator(int windowMs = 1000, QObject* parent = nullptr);

    void reset();

public slots:
    void onBLEShot();
    void onCameraShot(int score, int direction, const QString& imagePath);

signals:
    void shotFinalized(const ShotRecord& record);

private:
    void flushPending(int score, int direction, const QString& imagePath);
    void expirePending();

    int     m_windowMs;
    int     m_shotNumber     = 0;
    qint64  m_prevTime       = -1;
    bool    m_hasPending     = false;
    qint64  m_pendingBLETime = -1;
    QTimer* m_windowTimer    = nullptr;
};