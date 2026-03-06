#pragma once
// src/core/BluetoothManager.h
#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyDescriptor>
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
 * @brief Manages BLE scanning, device connections, and GMSync shot notifications.
 *
 * After connecting to a GMSync bracelet the manager automatically discovers the
 * B3A0 service, enables notifications on B3A1, and emits shotSignalReceived()
 * whenever the packet 0x55 0xAA 0x06 0x00 is received.
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

    bool    isScanning()          const { return m_isScanning;          }
    bool    isConnected()         const { return m_isConnected;         }
    QString connectedDeviceName() const { return m_connectedDeviceName; }
    QVector<BluetoothDevice> devices() const { return m_devices;        }

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

    // Emitted when the bracelet fires the 0x55 AA 06 00 shot notification
    void shotSignalReceived();

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

    // GMSync service discovery
    void onServiceDiscovered(const QBluetoothUuid& uuid);
    void onGmsyncServiceStateChanged(QLowEnergyService::ServiceState state);
    void onGmsyncCharacteristicChanged(const QLowEnergyCharacteristic& ch,
                                       const QByteArray& data);

private:
    void setupAgent();
    void setupDBusListener();
    void teardownController();

    // GMSync BLE protocol constants
    static const QString GmsyncServiceUuid;       // 0000b3a0-…
    static const QString GmsyncNotifyCharUuid;    // 0000b3a1-… (bracelet → app)
    static const QByteArray ShotPacket;           // 55 AA 06 00

    QBluetoothDeviceDiscoveryAgent*     m_agent       = nullptr;
    QLowEnergyController*               m_controller  = nullptr;
    QLowEnergyService*                  m_gmsyncService = nullptr;
    QBluetoothLocalDevice*              m_localDevice = nullptr;

    QVector<BluetoothDevice>            m_devices;
    QMap<QString, QBluetoothDeviceInfo> m_deviceInfos;

    bool    m_isScanning  = false;
    bool    m_isConnected = false;
    QString m_connectedDeviceName;
    QString m_connectedAddress;
};