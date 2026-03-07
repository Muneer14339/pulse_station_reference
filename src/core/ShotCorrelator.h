#pragma once
#include <QObject>
#include <QTimer>
#include "training/data/ReviewDataTypes.h"

class ShotCorrelator : public QObject {
    Q_OBJECT
public:
    // ── Tunables — change one line to adjust behaviour ─────────────────────
    static constexpr int BleDebounceMs       = 700;   // ignore duplicate BLE within this window (ms)
    static constexpr int CorrelationWindowMs = -1;    // auto-expire pending shot; -1 = wait forever
    static constexpr int SessionEndWaitMs    = 3000;  // max wait for pending shot on endSession (ms)
    // ───────────────────────────────────────────────────────────────────────

    explicit ShotCorrelator(QObject* parent = nullptr);

    void reset();

public slots:
    void onBLEShot();
    void onCameraShot(int score, int direction, const QString& imagePath);
    void beginDrain();   // call on end-session; emits drained() when safe to proceed

signals:
    void shotPending(int shotNumber);            // BLE fired — show waiting row in GUI
    void shotFinalized(const ShotRecord& record);
    void drained();                              // pending resolved; safe to teardown

private:
    void flushPending(int score, int direction, const QString& imagePath);
    void expirePending();

    int     m_shotNumber     = 0;
    qint64  m_prevTime       = -1;
    bool    m_hasPending     = false;
    qint64  m_pendingBLETime = -1;
    bool    m_debouncing     = false;
    bool    m_draining       = false;

    QTimer* m_windowTimer   = nullptr;   // shared: correlation window + drain timer
    QTimer* m_debounceTimer = nullptr;
};