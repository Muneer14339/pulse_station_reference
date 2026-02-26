#pragma once
#include <QObject>
#include <QString>

/**
 * @brief Holds the user's in-progress session configuration.
 *
 * Emits stateChanged() whenever any field changes so UI widgets
 * can re-render without tight coupling.
 *
 * resetAfter(stepLevel) clears every field that comes AFTER the given step,
 * ensuring downstream selections are wiped when an upstream choice changes.
 */
class SessionState : public QObject {
    Q_OBJECT
public:
    explicit SessionState(QObject* parent = nullptr);

    // ── Getters ──────────────────────────────────────────────────────────
    QString categoryId()       const { return m_categoryId; }
    QString caliberId()        const { return m_caliberId;  }
    QString profileId()        const { return m_profileId;  }
    int     distance()         const { return m_distance;   }
    QString drillId()          const { return m_drillId;    }
    bool    isBluetoothConnected() const { return m_bluetoothConnected; }

    // ── Setters ──────────────────────────────────────────────────────────
    void setCategoryId(const QString& id);
    void setCaliberId(const QString& id);
    void setProfileId(const QString& id);
    void setDistance(int dist);
    void setDrillId(const QString& id);
    void setBluetoothConnected(bool connected);

    // ── Helpers ──────────────────────────────────────────────────────────
    void reset();
    void resetAfter(int stepLevel);
    bool isComplete()   const;
    bool canProceed()   const;
    int  currentStep()  const;

signals:
    void stateChanged();
    void bluetoothConnectionChanged(bool connected);

private:
    QString m_categoryId;
    QString m_caliberId;
    QString m_profileId;
    int     m_distance = -1;
    QString m_drillId;
    bool    m_bluetoothConnected = false;
};
