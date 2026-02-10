#include "SessionState.h"

SessionState::SessionState(QObject* parent)
    : QObject(parent), m_distance(-1) {
}

void SessionState::setCategoryId(const QString& id) {
    if (m_categoryId != id) {
        resetAfter(1);
        m_categoryId = id;
        emit stateChanged();
    }
}

void SessionState::setCaliberId(const QString& id) {
    if (m_caliberId != id) {
        resetAfter(2);
        m_caliberId = id;
        emit stateChanged();
    }
}

void SessionState::setProfileId(const QString& id) {
    if (m_profileId != id) {
        resetAfter(3);
        m_profileId = id;
        emit stateChanged();
    }
}

void SessionState::setDistance(int dist) {
    if (m_distance != dist) {
        resetAfter(4);
        m_distance = dist;
        emit stateChanged();
    }
}

void SessionState::setTargetId(const QString& id) {
    if (m_targetId != id) {
        resetAfter(5);
        m_targetId = id;
        emit stateChanged();
    }
}

void SessionState::setDrillId(const QString& id) {
    if (m_drillId != id) {
        m_drillId = id;
        emit stateChanged();
    }
}

void SessionState::reset() {
    m_categoryId.clear();
    m_caliberId.clear();
    m_profileId.clear();
    m_distance = -1;
    m_targetId.clear();
    m_drillId.clear();
    emit stateChanged();
}

void SessionState::resetAfter(int stepLevel) {
    if (stepLevel < 2) m_caliberId.clear();
    if (stepLevel < 3) m_profileId.clear();
    if (stepLevel < 4) m_distance = -1;
    if (stepLevel < 5) m_targetId.clear();
    if (stepLevel < 6) m_drillId.clear();
}

bool SessionState::isComplete() const {
    return !m_categoryId.isEmpty() && !m_caliberId.isEmpty() && 
           !m_profileId.isEmpty() && m_distance != -1 && 
           !m_targetId.isEmpty() && !m_drillId.isEmpty();
}

int SessionState::currentStep() const {
    if (m_drillId.isEmpty() == false) return 6;
    if (m_targetId.isEmpty() == false) return 5;
    if (m_distance != -1) return 4;
    if (m_profileId.isEmpty() == false) return 3;
    if (m_caliberId.isEmpty() == false) return 2;
    if (m_categoryId.isEmpty() == false) return 1;
    return 0;
}
