#pragma once
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QProcess>
#include "core/BluetoothManager.h"

/**
 * @brief Left-panel widget for BLE device discovery and connection.
 *
 * Shows scanning state, discovered devices, and connection status.
 * Emits connectionChanged(bool) so ConsoleWidget can update SessionState.
 */
class BluetoothPanel : public QWidget {
    Q_OBJECT
public:
    explicit BluetoothPanel(BluetoothManager* btManager, QWidget* parent = nullptr);

signals:
    void connectionChanged(bool connected);

private:
    void buildUI();

    // Slots connected to BluetoothManager signals
    void onDeviceFound(const BluetoothDevice& device);
    void onDevicesCleared();
    void onScanningStarted();
    void onScanningStopped();
    void onConnecting();
    void onConnected(const QString& deviceName);
    void onDisconnected();
    void onBluetoothPoweredChanged(bool powered);

    BluetoothManager* m_btManager;

    // Header row widgets
    QWidget*     m_headerRow;
    QLabel*      m_headerLabel;
    QPushButton* m_refreshBtn;

    // Offline state
    QWidget*     m_offlinePanel;
    QPushButton* m_bluetoothBtn;

    // Device list
    QScrollArea* m_devicesScroll;
    QWidget*     m_devicesContainer;
    QVBoxLayout* m_devicesLayout;
    QWidget*     m_emptyState;
    QWidget*     m_scanLoader;
    QWidget*     m_connectLoader;

    // Help panel
    QWidget*     m_helpPanel;

    // Animated dot loader
    QTimer* m_dotTimer;
    int     m_dotCount = 0;

    // Currently shown device widgets keyed by BT address
    QMap<QString, QWidget*> m_deviceWidgets;
};
