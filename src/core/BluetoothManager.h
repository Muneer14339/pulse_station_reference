#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QBluetoothLocalDevice>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QXmlStreamReader>
#include <QProcess>

struct BluetoothDevice {
    QString name;
    QString address;
    int     rssi = 0;
};

/**
 * @brief Manages BLE scanning and device connections.
 *
 * Signals are emitted for all state transitions so UI panels can react
 * without polling.  After a disconnect the manager automatically restarts
 * a fresh scan.
 */
class BluetoothManager : public QObject {
    Q_OBJECT
public:
    explicit BluetoothManager(QObject* parent = nullptr);
    ~BluetoothManager();

    void startScanning();
    void stopScanning();
    void restartScanning();
    void connectToDevice(const QString& address);
    void disconnectDevice();

    bool    isScanning()          const { return m_isScanning;         }
    bool    isConnected()         const { return m_isConnected;        }
    QString connectedDeviceName() const { return m_connectedDeviceName;}
    QVector<BluetoothDevice> devices() const { return m_devices;       }

    bool isBluetoothPowered() const;
    void setBluetoothPowered(bool on);

signals:
    void scanningStarted();
    void scanningStopped();
    void deviceFound(const BluetoothDevice& device);
    void devicesCleared();
    void connecting();
    void connected(const QString& deviceName);
    void disconnected();
    void error(const QString& message);
    void bluetoothPoweredChanged(bool powered);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo& info);
    void onScanFinished();
    void onScanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void onControllerConnected();
    void onControllerDisconnected();
    void onControllerError(QLowEnergyController::Error error);
    void onBluezPropertiesChanged(const QString& interface,
                                  const QVariantMap& changed,
                                  const QStringList& invalidated);

private:
    void setupAgent();
    void setupDBusListener();
    void teardownController();

    QBluetoothDeviceDiscoveryAgent*     m_agent      = nullptr;
    QLowEnergyController*               m_controller = nullptr;
    QBluetoothLocalDevice*              m_localDevice = nullptr;

    QVector<BluetoothDevice>            m_devices;
    QMap<QString, QBluetoothDeviceInfo> m_deviceInfos;

    bool    m_isScanning  = false;
    bool    m_isConnected = false;
    QString m_connectedDeviceName;
    QString m_connectedAddress;
};
