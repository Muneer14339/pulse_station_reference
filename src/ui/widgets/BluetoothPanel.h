#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProcess>
#include "core/BluetoothManager.h"
#include "common/ScanPanel.h"

/**
 * @brief BLE device panel. Wraps ScanPanel and bridges BluetoothManager signals.
 */
class BluetoothPanel : public QWidget {
    Q_OBJECT
public:
    explicit BluetoothPanel(BluetoothManager* btManager, QWidget* parent = nullptr);

signals:
    void connectionChanged(bool connected);

private:
    void onDeviceFound(const BluetoothDevice& device);
    void onDevicesCleared();
    void onScanningStarted();
    void onScanningStopped();
    void onConnecting();
    void onConnected(const QString& deviceName);
    void onDisconnected();
    void onBluetoothPoweredChanged(bool powered);

    BluetoothManager* m_btManager;
    ScanPanel*        m_scanPanel;
    QWidget*          m_offlinePanel;
    QWidget*          m_helpPanel;
};