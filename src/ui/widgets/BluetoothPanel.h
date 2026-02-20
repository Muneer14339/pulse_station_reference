#pragma once
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QProcess>
#include "core/BluetoothManager.h"

class BluetoothPanel : public QWidget {
    Q_OBJECT
public:
    explicit BluetoothPanel(BluetoothManager* btManager, QWidget* parent = nullptr);
    
signals:
    void connectionChanged(bool connected);
    
private:
    void setupUI();
    void updateUI();
    void onDeviceFound(const BluetoothDevice& device);
    void onDevicesCleared();
    void onScanningStarted();
    void onScanningStopped();
    void onConnecting();
    void onConnected(const QString& deviceName);
    void onDisconnected();
    void onBluetoothPoweredChanged(bool powered);
    
    BluetoothManager* m_btManager;
    
    QLabel* m_headerLabel;
    QPushButton* m_refreshBtn;
    QPushButton* m_bluetoothBtn;
    QWidget* m_helpPanel;
    QWidget* m_devicesContainer;
    QVBoxLayout* m_devicesLayout;
    QScrollArea* m_devicesScroll;
    
    QMap<QString, QWidget*> m_deviceWidgets;
    QWidget* m_offlinePanel;
    QWidget* m_headerRow;
};