#include "BluetoothManager.h"
#include <QTimer>

BluetoothManager::BluetoothManager(QObject* parent)
    : QObject(parent), m_isScanning(false), m_isConnected(false) {
    
    m_agent = new QBluetoothDeviceDiscoveryAgent(this);
    m_agent->setLowEnergyDiscoveryTimeout(5000);
    
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothManager::onDeviceDiscovered);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BluetoothManager::onScanFinished);
    connect(m_agent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
            this, &BluetoothManager::onScanError);
}

void BluetoothManager::startScanning() {
    if (m_isScanning || m_isConnected) return;
    
    m_devices.clear();
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

void BluetoothManager::connectToDevice(const QString& address) {
    if (m_isConnected) return;
    
    stopScanning();
    
    for (const BluetoothDevice& dev : m_devices) {
        if (dev.address == address) {
            m_connectedAddress = address;
            m_connectedDeviceName = dev.name;
            break;
        }
    }
    
    emit connecting();
    
    QTimer::singleShot(1500, this, [this]() {
        m_isConnected = true;
        emit connected(m_connectedDeviceName);
    });
}

void BluetoothManager::disconnectDevice() {
    if (!m_isConnected) return;
    
    m_isConnected = false;
    m_connectedDeviceName.clear();
    m_connectedAddress.clear();
    
    emit disconnected();
    
    QTimer::singleShot(500, this, [this]() {
        startScanning();
    });
}

void BluetoothManager::onDeviceDiscovered(const QBluetoothDeviceInfo& info) {
    QString name = info.name();
    if (name.isEmpty()) name = "Unknown Device";
    
    QString address = info.address().toString();
    for (const BluetoothDevice& dev : m_devices) {
        if (dev.address == address) return;
    }
    
    BluetoothDevice device;
    device.name = name;
    device.address = address;
    device.rssi = info.rssi();
    
    m_devices.append(device);
    emit deviceFound(device);
}

void BluetoothManager::onScanFinished() {
    m_isScanning = false;
    emit scanningStopped();
}

void BluetoothManager::onScanError(QBluetoothDeviceDiscoveryAgent::Error error) {
    m_isScanning = false;
    emit this->error("Bluetooth scan error");
    emit scanningStopped();
}