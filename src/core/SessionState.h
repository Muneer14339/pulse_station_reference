#pragma once
#include <QObject>
#include <QString>

class SessionState : public QObject {
    Q_OBJECT
public:
    explicit SessionState(QObject* parent = nullptr);

    QString categoryId()           const { return m_categoryId;        }
    QString caliberId()            const { return m_caliberId;         }
    QString profileId()            const { return m_profileId;         }
    int     distance()             const { return m_distance;          }
    QString drillId()              const { return m_drillId;           }
    bool    isBluetoothConnected() const { return m_bluetoothConnected;}
    bool    isCameraConnected()    const { return m_cameraConnected;   }
    int     cameraIndex()          const { return m_cameraIndex;       }

    void setCategoryId(const QString& id);
    void setCaliberId(const QString& id);
    void setProfileId(const QString& id);
    void setDistance(int dist);
    void setDrillId(const QString& id);
    void setBluetoothConnected(bool connected);
    void setCameraConnected(bool connected);
    void setCameraIndex(int index)         { m_cameraIndex = index; }

    void reset();
    void resetAfter(int stepLevel);
    bool isComplete()       const;
    bool canProceed()       const; // BLE only  → ConsoleWidget Next button
    bool canStartTraining() const; // BLE+Camera → TrainingPlaceholder Start button

    int  currentStep() const;

signals:
    void stateChanged();
    void bluetoothConnectionChanged(bool connected);
    void cameraConnectionChanged(bool connected);

private:
    QString m_categoryId;
    QString m_caliberId;
    QString m_profileId;
    int     m_distance           = -1;
    QString m_drillId;
    bool    m_bluetoothConnected = false;
    bool    m_cameraConnected    = false;
    int     m_cameraIndex        = -1;
};