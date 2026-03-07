#include "ShotCorrelator.h"
#include <QDateTime>

ShotCorrelator::ShotCorrelator(int windowMs, QObject* parent)
    : QObject(parent), m_windowMs(windowMs)
{
    m_windowTimer = new QTimer(this);
    m_windowTimer->setSingleShot(true);
    connect(m_windowTimer, &QTimer::timeout, this, &ShotCorrelator::expirePending);
}

void ShotCorrelator::reset() {
    m_windowTimer->stop();
    m_hasPending     = false;
    m_pendingBLETime = -1;
    m_shotNumber     = 0;
    m_prevTime       = -1;
}

void ShotCorrelator::onBLEShot() {
    // Previous BLE shot still pending (rapid fire / camera miss) — expire it first
    if (m_hasPending) {
        m_windowTimer->stop();
        expirePending();
    }
    m_hasPending     = true;
    m_pendingBLETime = QDateTime::currentMSecsSinceEpoch();
    m_windowTimer->start(m_windowMs);
}

void ShotCorrelator::onCameraShot(int score, int direction, const QString& imagePath) {
    if (m_hasPending) {
        // Normal case: BLE arrived first, camera confirmed the shot
        m_windowTimer->stop();
        flushPending(score, direction, imagePath);
    } else {
        // Camera-only: no BLE signal was pending
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
}

void ShotCorrelator::expirePending() {
    // BLE fired but camera never responded within the correlation window
    ShotRecord rec;
    rec.number    = ++m_shotNumber;
    rec.score     = 0;
    rec.splitTime = (m_prevTime < 0) ? -1.0 : (m_pendingBLETime - m_prevTime) / 1000.0;
    // direction stays empty — missed shot has no placement data
    m_prevTime       = m_pendingBLETime;
    m_hasPending     = false;
    m_pendingBLETime = -1;
    emit shotFinalized(rec);
}