#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QShowEvent>
#include "common/ScanPanel.h"
#include "training/engine/dart_system.h"

class CameraPanel : public QWidget {
    Q_OBJECT
public:
    // Change this index to target a different system camera in the future.
    static constexpr int kAutoConnectIndex = 3;

    explicit CameraPanel(QWidget* parent = nullptr);

    int  selectedCameraIndex() const { return m_selectedIndex; }
    bool isConnected()         const { return m_connected;     }

signals:
    void connectionChanged(bool connected, int cameraIndex);

protected:
    void showEvent(QShowEvent* event) override;

private:
    void scan();
    void autoConnect(int cameraIndex, const QString& name);

    ScanPanel* m_scanPanel;
    int        m_selectedIndex = -1;
    bool       m_connected     = false;
    bool       m_firstShow     = true;
};