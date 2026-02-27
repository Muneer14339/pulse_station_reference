#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QShowEvent>
#include "common/ScanPanel.h"
#include "training/engine/dart_system.h"

/**
 * @brief Camera device discovery panel.
 * Scan triggers only when widget becomes visible (showEvent),
 * NOT at construction time — avoids running on app startup.
 */
class CameraPanel : public QWidget {
    Q_OBJECT
public:
    explicit CameraPanel(QWidget* parent = nullptr);

    int  selectedCameraIndex() const { return m_selectedIndex; }
    bool isConnected()         const { return m_connected;     }

signals:
    void connectionChanged(bool connected, int cameraIndex);

protected:
    void showEvent(QShowEvent* event) override;

private:
    void scan();
    void onConnectClicked(const QString& id);
    void onDisconnectClicked();

    ScanPanel* m_scanPanel;
    int        m_selectedIndex  = -1;
    bool       m_connected      = false;
    bool       m_firstShow      = true;
};