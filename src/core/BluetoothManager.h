#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

struct BluetoothDevice {
    QString name;
    QString address;
    int rssi;
};

class BluetoothManager : public QObject {
    Q_OBJECT
public:
    explicit BluetoothManager(QObject* parent = nullptr);
    ~BluetoothManager();

    void startScanning();
    void stopScanning();
    void restartScanning();          // force-clears list and starts fresh
    void connectToDevice(const QString& address);
    void disconnectDevice();

    bool isScanning()  const { return m_isScanning;  }
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

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo& info);
    void onScanFinished();
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void onControllerConnected();
    void onControllerDisconnected();
    void onControllerError(QLowEnergyController::Error error);

private:
    void teardownController();

    QBluetoothDeviceDiscoveryAgent*         m_agent;
    QLowEnergyController*                   m_controller = nullptr;

    QVector<BluetoothDevice>                m_devices;
    QMap<QString, QBluetoothDeviceInfo>     m_deviceInfos;   // address -> full info

    bool    m_isScanning  = false;
    bool    m_isConnected = false;
    QString m_connectedDeviceName;
    QString m_connectedAddress;
};