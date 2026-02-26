#include "BluetoothManager.h"
#include <QTimer>

// ── Construction / Destruction ────────────────────────────────────────────────

BluetoothManager::BluetoothManager(QObject* parent)
    : QObject(parent)
{
    setupAgent();

    m_localDevice = new QBluetoothLocalDevice(this);

    // Listen for adapter power changes via D-Bus
    QDBusConnection::systemBus().connect(
        "org.bluez", "/org/bluez/hci0",
        "org.freedesktop.DBus.Properties", "PropertiesChanged",
        this, SLOT(onBluezPropertiesChanged(QString, QVariantMap, QStringList)));

    connect(m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged,
            this, [this](QBluetoothLocalDevice::HostMode mode) {
        const bool powered = (mode != QBluetoothLocalDevice::HostPoweredOff);
        emit bluetoothPoweredChanged(powered);
        if (powered) {
            startScanning();
        } else {
            stopScanning();
            m_devices.clear();
            m_deviceInfos.clear();
            emit devicesCleared();
        }
    });
}

BluetoothManager::~BluetoothManager() {
    teardownController();
}

// ── Scanning ──────────────────────────────────────────────────────────────────

void BluetoothManager::startScanning() {
    if (m_isScanning || m_isConnected) return;

    m_devices.clear();
    m_deviceInfos.clear();
    emit devicesCleared();

    m_isScanning = true;
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    emit scanningStarted();
}

void BluetoothManager::stopScanning() {
    if (!m_isScanning) return;
    m_agent->stop();
    m_isScanning = false;
    emit scanningStopped();
}

void BluetoothManager::restartScanning() {
    if (m_isConnected) return;

    m_isScanning = false;
    m_agent->stop();

    for (const BluetoothDevice& dev : m_devices)
        QProcess::execute("bluetoothctl", {"remove", dev.address});

    m_devices.clear();
    m_deviceInfos.clear();
    emit devicesCleared();

    setupAgent();

    m_isScanning = true;
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    emit scanningStarted();
}

// ── Connection ────────────────────────────────────────────────────────────────

void BluetoothManager::connectToDevice(const QString& address) {
    if (m_isConnected || !m_deviceInfos.contains(address)) return;

    stopScanning();

    for (const BluetoothDevice& dev : m_devices) {
        if (dev.address == address) {
            m_connectedAddress    = address;
            m_connectedDeviceName = dev.name;
            break;
        }
    }

    emit connecting();
    teardownController();

    m_controller = QLowEnergyController::createCentral(m_deviceInfos[address], this);

    connect(m_controller, &QLowEnergyController::connected,
            this,          &BluetoothManager::onControllerConnected);
    connect(m_controller, &QLowEnergyController::disconnected,
            this,          &BluetoothManager::onControllerDisconnected);
    connect(m_controller,
            QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error),
            this, &BluetoothManager::onControllerError);

    m_controller->connectToDevice();
}

void BluetoothManager::disconnectDevice() {
    if (!m_isConnected || !m_controller) return;
    m_controller->disconnectFromDevice();
}

// ── Discovery callbacks ───────────────────────────────────────────────────────

void BluetoothManager::onDeviceDiscovered(const QBluetoothDeviceInfo& info) {
    if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration))
        return;

    const QString address = info.address().toString();
    if (m_deviceInfos.contains(address)) return;

    m_deviceInfos[address] = info;

    BluetoothDevice device;
    device.name    = info.name().isEmpty() ? "Unknown Device" : info.name();
    device.address = address;
    device.rssi    = info.rssi();

    m_devices.append(device);
    emit deviceFound(device);
}

void BluetoothManager::onScanFinished() {
    m_isScanning = false;
    emit scanningStopped();
}

void BluetoothManager::onScanError(QBluetoothDeviceDiscoveryAgent::Error /*error*/) {
    m_isScanning = false;
    emit error("Bluetooth scan error");
    emit scanningStopped();
}

// ── Controller callbacks ──────────────────────────────────────────────────────

void BluetoothManager::onControllerConnected() {
    m_isConnected = true;
    emit connected(m_connectedDeviceName);
}

void BluetoothManager::onControllerDisconnected() {
    m_isConnected = false;
    m_connectedDeviceName.clear();
    m_connectedAddress.clear();
    teardownController();
    emit disconnected();
    QTimer::singleShot(500, this, [this]() { restartScanning(); });
}

void BluetoothManager::onControllerError(QLowEnergyController::Error /*error*/) {
    m_isConnected = false;
    m_connectedDeviceName.clear();
    m_connectedAddress.clear();
    teardownController();
    emit error("Connection failed");
    emit disconnected();
    QTimer::singleShot(500, this, [this]() { restartScanning(); });
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void BluetoothManager::teardownController() {
    if (!m_controller) return;
    m_controller->disconnect();
    m_controller->deleteLater();
    m_controller = nullptr;
}

void BluetoothManager::setupAgent() {
    if (m_agent) {
        m_agent->stop();
        m_agent->disconnect();
        m_agent->deleteLater();
    }

    m_agent = new QBluetoothDeviceDiscoveryAgent(this);
    m_agent->setLowEnergyDiscoveryTimeout(5000);

    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this,    &BluetoothManager::onDeviceDiscovered);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished,
            this,    &BluetoothManager::onScanFinished);
    connect(m_agent,
            QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
            this, &BluetoothManager::onScanError);
}

bool BluetoothManager::isBluetoothPowered() const {
    return m_localDevice
        && m_localDevice->hostMode() != QBluetoothLocalDevice::HostPoweredOff;
}

void BluetoothManager::setBluetoothPowered(bool /*on*/) {
    if (m_localDevice) m_localDevice->powerOn();
}

void BluetoothManager::onBluezPropertiesChanged(const QString& interface,
                                                 const QVariantMap& changed,
                                                 const QStringList& /*invalidated*/)
{
    if (interface != "org.bluez.Adapter1" || !changed.contains("Powered")) return;

    const bool powered = changed.value("Powered").toBool();
    emit bluetoothPoweredChanged(powered);
    if (powered) {
        startScanning();
    } else {
        stopScanning();
        m_devices.clear();
        m_deviceInfos.clear();
        emit devicesCleared();
    }
}

void BluetoothManager::setupDBusListener() {
    QDBusInterface bluez("org.bluez", "/org/bluez",
                         "org.freedesktop.DBus.Introspectable",
                         QDBusConnection::systemBus());
    QDBusReply<QString> reply = bluez.call("Introspect");
    if (!reply.isValid()) return;

    QXmlStreamReader xml(reply.value());
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QLatin1String("node")) {
            const QString name = xml.attributes().value("name").toString();
            if (name.startsWith("hci")) {
                QDBusConnection::systemBus().connect(
                    "org.bluez", "/org/bluez/" + name,
                    "org.freedesktop.DBus.Properties", "PropertiesChanged",
                    this, SLOT(onBluezPropertiesChanged(QString, QVariantMap, QStringList)));
            }
        }
    }
}
