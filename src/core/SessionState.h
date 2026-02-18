#pragma once
#include <QObject>
#include <QString>

class SessionState : public QObject {
    Q_OBJECT
public:
    explicit SessionState(QObject* parent = nullptr);
    
    QString categoryId() const { return m_categoryId; }
    QString caliberId() const { return m_caliberId; }
    QString profileId() const { return m_profileId; }
    int distance() const { return m_distance; }
    QString targetId() const { return m_targetId; }
    QString drillId() const { return m_drillId; }
    bool isBluetoothConnected() const { return m_bluetoothConnected; }
    
    void setCategoryId(const QString& id);
    void setCaliberId(const QString& id);
    void setProfileId(const QString& id);
    void setDistance(int dist);
    void setTargetId(const QString& id);
    void setDrillId(const QString& id);
    void setBluetoothConnected(bool connected);
    
    void reset();
    void resetAfter(int stepLevel);
    bool isComplete() const;
    bool canProceed() const;
    int currentStep() const;
    
signals:
    void stateChanged();
    void bluetoothConnectionChanged(bool connected);
    
private:
    QString m_categoryId;
    QString m_caliberId;
    QString m_profileId;
    int m_distance;
    QString m_targetId;
    QString m_drillId;
    bool m_bluetoothConnected;
};