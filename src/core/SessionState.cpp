#include "SessionState.h"

SessionState::SessionState(QObject* parent) : QObject(parent) {}

void SessionState::setCategoryId(const QString& id) {
    if (m_categoryId == id) return;
    resetAfter(1); m_categoryId = id; emit stateChanged();
}
void SessionState::setCaliberId(const QString& id) {
    if (m_caliberId == id) return;
    resetAfter(2); m_caliberId = id; emit stateChanged();
}
void SessionState::setProfileId(const QString& id) {
    if (m_profileId == id) return;
    resetAfter(3); m_profileId = id; emit stateChanged();
}
void SessionState::setDistance(int dist) {
    if (m_distance == dist) return;
    resetAfter(4); m_distance = dist; emit stateChanged();
}
void SessionState::setDrillId(const QString& id) {
    if (m_drillId == id) return;
    m_drillId = id; emit stateChanged();
}
void SessionState::setBluetoothConnected(bool connected) {
    if (m_bluetoothConnected == connected) return;
    m_bluetoothConnected = connected;
    emit bluetoothConnectionChanged(connected);
    emit stateChanged();
}
void SessionState::setCameraConnected(bool connected) {
    if (m_cameraConnected == connected) return;
    m_cameraConnected = connected;
    emit cameraConnectionChanged(connected);
    emit stateChanged();
}

void SessionState::reset() {
    m_categoryId.clear(); m_caliberId.clear();
    m_profileId.clear();  m_distance = -1;
    m_drillId.clear();    emit stateChanged();
}

void SessionState::resetAfter(int stepLevel) {
    if (stepLevel < 2) m_caliberId.clear();
    if (stepLevel < 3) m_profileId.clear();
    if (stepLevel < 4) m_distance = -1;
    if (stepLevel < 5) m_drillId.clear();
}

bool SessionState::isComplete() const {
    return !m_categoryId.isEmpty() && !m_caliberId.isEmpty()
        && !m_profileId.isEmpty()  && m_distance != -1
        && !m_drillId.isEmpty();
}

// Console screen: only BLE required
bool SessionState::canProceed() const {
    return m_bluetoothConnected && isComplete();
}

// Training screen: both BLE + camera required
bool SessionState::canStartTraining() const {
    return m_bluetoothConnected && m_cameraConnected && isComplete();
}

int SessionState::currentStep() const {
    if (!m_drillId.isEmpty())    return 5;
    if (m_distance != -1)        return 4;
    if (!m_profileId.isEmpty())  return 3;
    if (!m_caliberId.isEmpty())  return 2;
    if (!m_categoryId.isEmpty()) return 1;
    return 0;
}