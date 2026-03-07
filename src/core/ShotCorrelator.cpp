#include "ShotCorrelator.h"
#include <QDateTime>

ShotCorrelator::ShotCorrelator(QObject* parent) : QObject(parent) {
    m_windowTimer = new QTimer(this);
    m_windowTimer->setSingleShot(true);
    connect(m_windowTimer, &QTimer::timeout, this, &ShotCorrelator::expirePending);

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    connect(m_debounceTimer, &QTimer::timeout, this, [this]{ m_debouncing = false; });
}

void ShotCorrelator::reset() {
    m_windowTimer->stop();
    m_debounceTimer->stop();
    m_hasPending     = false;
    m_debouncing     = false;
    m_draining       = false;
    m_pendingBLETime = -1;
    m_shotNumber     = 0;
    m_prevTime       = -1;
}

void ShotCorrelator::onBLEShot() {
    if (m_debouncing) return;
    m_debouncing = true;
    m_debounceTimer->start(BleDebounceMs);

    // Previous pending never got camera data → expire as missed before opening new window
    if (m_hasPending) {
        m_windowTimer->stop();
        expirePending();
    }

    m_hasPending     = true;
    m_pendingBLETime = QDateTime::currentMSecsSinceEpoch();
    emit shotPending(m_shotNumber + 1);   // preview number (not yet incremented)

    if (CorrelationWindowMs > 0)
        m_windowTimer->start(CorrelationWindowMs);
}

void ShotCorrelator::onCameraShot(int score, int direction, const QString& imagePath) {
    if (m_hasPending) {
        m_windowTimer->stop();
        flushPending(score, direction, imagePath);
    } else {
        // Camera-only shot (no BLE pending)
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        ShotRecord rec;
        rec.number    = ++m_shotNumber;
        rec.score     = score;
        rec.splitTime = (m_prevTime < 0) ? -1.0 : (now - m_prevTime) / 1000.0;
        rec.direction = direction ? QString::number(direction) : QString();
        rec.imagePath = imagePath;
        m_prevTime    = now;
        emit shotFinalized(rec);
    }
}

void ShotCorrelator::beginDrain() {
    if (!m_hasPending) { emit drained(); return; }

    m_draining = true;
    m_windowTimer->stop();
    const qint64 elapsed   = QDateTime::currentMSecsSinceEpoch() - m_pendingBLETime;
    const int    remaining = static_cast<int>(qMax(0LL, (qint64)SessionEndWaitMs - elapsed));
    m_windowTimer->start(remaining);
}

void ShotCorrelator::flushPending(int score, int direction, const QString& imagePath) {
    ShotRecord rec;
    rec.number    = ++m_shotNumber;
    rec.score     = score;
    rec.splitTime = (m_prevTime < 0) ? -1.0 : (m_pendingBLETime - m_prevTime) / 1000.0;
    rec.direction = direction ? QString::number(direction) : QString();
    rec.imagePath = imagePath;
    m_prevTime       = m_pendingBLETime;
    m_hasPending     = false;
    m_pendingBLETime = -1;
    emit shotFinalized(rec);

    if (m_draining) { m_draining = false; emit drained(); }
}

void ShotCorrelator::expirePending() {
    ShotRecord rec;
    rec.number    = ++m_shotNumber;
    rec.score     = 0;
    rec.missed    = true;
    rec.splitTime = (m_prevTime < 0) ? -1.0 : (m_pendingBLETime - m_prevTime) / 1000.0;
    m_prevTime       = m_pendingBLETime;
    m_hasPending     = false;
    m_pendingBLETime = -1;
    emit shotFinalized(rec);

    if (m_draining) { m_draining = false; emit drained(); }
}