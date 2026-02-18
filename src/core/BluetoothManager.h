#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

struct BluetoothDevice {
    QString name;
    QString address;
    int rssi;
};

class BluetoothManager : public QObject {
    Q_OBJECT
public:
    explicit BluetoothManager(QObject* parent = nullptr);
    
    void startScanning();
    void stopScanning();
    void connectToDevice(const QString& address);
    void disconnectDevice();
    
    bool isScanning() const { return m_isScanning; }
    bool isConnected() const { return m_isConnected; }
    QString connectedDeviceName() const { return m_connectedDeviceName; }
    QVector<BluetoothDevice> devices() const { return m_devices; }
    
signals:
    void scanningStarted();
    void scanningStopped();
    void deviceFound(const BluetoothDevice& device);
    void devicesCleared();
    void connecting();
    void connected(const QString& deviceName);
    void disconnected();
    void error(const QString& message);
    
private:
    void onDeviceDiscovered(const QBluetoothDeviceInfo& info);
    void onScanFinished();
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error error);
    
    QBluetoothDeviceDiscoveryAgent* m_agent;
    QVector<BluetoothDevice> m_devices;
    bool m_isScanning;
    bool m_isConnected;
    QString m_connectedDeviceName;
    QString m_connectedAddress;
};