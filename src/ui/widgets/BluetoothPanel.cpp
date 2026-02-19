#include "BluetoothPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"

BluetoothPanel::BluetoothPanel(BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_btManager(btManager) {
    setupUI();
    
    connect(m_btManager, &BluetoothManager::deviceFound, this, &BluetoothPanel::onDeviceFound);
    connect(m_btManager, &BluetoothManager::devicesCleared, this, &BluetoothPanel::onDevicesCleared);
    connect(m_btManager, &BluetoothManager::scanningStarted, this, &BluetoothPanel::onScanningStarted);
    connect(m_btManager, &BluetoothManager::scanningStopped, this, &BluetoothPanel::onScanningStopped);
    connect(m_btManager, &BluetoothManager::connecting, this, &BluetoothPanel::onConnecting);
    connect(m_btManager, &BluetoothManager::connected, this, &BluetoothPanel::onConnected);
    connect(m_btManager, &BluetoothManager::disconnected, this, &BluetoothPanel::onDisconnected);
    
    m_btManager->startScanning();
}

void BluetoothPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 16);
    layout->setSpacing(10);
    
    auto* headerRow = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(headerRow);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    m_headerLabel = new QLabel("Bluetooth Connections", headerRow);
    m_headerLabel->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
    
    m_refreshBtn = new QPushButton("↻", headerRow);
    m_refreshBtn->setStyleSheet(
        "QPushButton { background: transparent; border: 1px solid rgba(79, 209, 197, 128); "
        "border-radius: 6px; color: rgb(79, 209, 197); padding: 4px 8px; font-size: 16px; }"
        "QPushButton:hover { background: rgba(79, 209, 197, 26); }"
    );
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedSize(32, 32);
    
    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshBtn);
    headerRow->setLayout(headerLayout);
    
    m_bluetoothBtn = new QPushButton("Bluetooth: ON", this);
    m_bluetoothBtn->setStyleSheet(
        "QPushButton { background: rgb(79, 209, 197); border: none; border-radius: 8px; "
        "color: rgb(10, 15, 25); font-weight: 600; font-size: 13px; padding: 10px; }"
    );
    m_bluetoothBtn->setCursor(Qt::PointingHandCursor);
    
    m_helpPanel = new QWidget(this);
    m_helpPanel->setStyleSheet("background: rgba(9, 14, 27, 153); border: 1px solid rgba(255,255,255,51); border-radius: 10px; padding: 10px;");
    auto* helpLayout = new QVBoxLayout(m_helpPanel);
    helpLayout->setContentsMargins(10, 10, 10, 10);
    helpLayout->setSpacing(6);
    
    auto* helpTitle = new QLabel("If device is not listed", m_helpPanel);
    helpTitle->setStyleSheet("font-size: 12px; font-weight: 600; color: rgb(199, 204, 240); background: transparent; border: none;");
    
    auto* help1 = new QLabel("• Ensure the AimSync device is on.", m_helpPanel);
    help1->setStyleSheet("font-size: 11px; color: rgb(165, 172, 201); background: transparent; border: none;");
    help1->setWordWrap(true);
    
    auto* help2 = new QLabel("• Restart the AimSync device.", m_helpPanel);
    help2->setStyleSheet("font-size: 11px; color: rgb(165, 172, 201); background: transparent; border: none;");
    help2->setWordWrap(true);
    
    auto* help3 = new QLabel("• If issue is not fixed contact at", m_helpPanel);
    help3->setStyleSheet("font-size: 11px; color: rgb(165, 172, 201); background: transparent; border: none;");
    help3->setWordWrap(true);
    
    auto* helpEmail = new QLabel("  support@pulsestation.com", m_helpPanel);
    helpEmail->setStyleSheet("font-size: 11px; color: rgb(79, 209, 197); background: transparent; border: none;");
    
    helpLayout->addWidget(helpTitle);
    helpLayout->addWidget(help1);
    helpLayout->addWidget(help2);
    helpLayout->addWidget(help3);
    helpLayout->addWidget(helpEmail);
    m_helpPanel->setLayout(helpLayout);
    
    m_devicesScroll = new QScrollArea(this);
    m_devicesScroll->setWidgetResizable(true);
    m_devicesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_devicesScroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    m_devicesContainer = new QWidget();
    m_devicesLayout = new QVBoxLayout(m_devicesContainer);
    m_devicesLayout->setContentsMargins(0, 0, 0, 0);
    m_devicesLayout->setSpacing(8);
    m_devicesLayout->addStretch();
    m_devicesContainer->setLayout(m_devicesLayout);
    
    m_devicesScroll->setWidget(m_devicesContainer);
    
    layout->addWidget(headerRow);
    layout->addWidget(m_bluetoothBtn);
    layout->addWidget(m_devicesScroll);
    layout->addWidget(m_helpPanel);
    setLayout(layout);
    
    connect(m_refreshBtn, &QPushButton::clicked, [this]() {
        if (!m_btManager->isConnected()) m_btManager->startScanning();
    });
}

void BluetoothPanel::onDeviceFound(const BluetoothDevice& device) {
    // 1. Sirf allowed devices filter karo
    static const QStringList allowed = {"GMSync", "RA", "SK", "PulseStation"};
    bool valid = false;
    for (const QString& prefix : allowed)
        if (device.name.contains(prefix, Qt::CaseInsensitive)) { valid = true; break; }
    if (!valid) return;

    if (m_deviceWidgets.contains(device.address)) return;

    auto* deviceWidget = new QWidget(m_devicesContainer);
    deviceWidget->setStyleSheet(
        "QWidget { background: rgba(13, 19, 40, 204); border: 1px solid rgba(255,255,255,51); border-radius: 10px; }"
    );

    auto* deviceLayout = new QHBoxLayout(deviceWidget);
    deviceLayout->setContentsMargins(12, 10, 12, 10);

    // 2. Name + Address dono dikhao
    auto* infoBlock = new QWidget(deviceWidget);
    infoBlock->setStyleSheet("background: transparent; border: none;");
    auto* infoLayout = new QVBoxLayout(infoBlock);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(2);

    auto* nameLabel = new QLabel(device.name, infoBlock);
    nameLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");

    auto* addrLabel = new QLabel(device.address, infoBlock);
    addrLabel->setStyleSheet("font-size: 10px; color: rgb(113, 120, 164); background: transparent; border: none;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(addrLabel);
    infoBlock->setLayout(infoLayout);

    auto* connectBtn = new QPushButton("Connect", deviceWidget);
    connectBtn->setStyleSheet(
        "QPushButton { background: transparent; border: 1px solid rgb(79, 209, 197); "
        "border-radius: 8px; color: rgb(79, 209, 197); padding: 6px 14px; font-size: 12px; font-weight: 600; }"
        "QPushButton:hover { background: rgba(79, 209, 197, 26); }"
    );
    connectBtn->setCursor(Qt::PointingHandCursor);

    deviceLayout->addWidget(infoBlock);
    deviceLayout->addStretch();
    deviceLayout->addWidget(connectBtn);
    deviceWidget->setLayout(deviceLayout);

    m_devicesLayout->insertWidget(m_devicesLayout->count() - 1, deviceWidget);
    m_deviceWidgets[device.address] = deviceWidget;

    connect(connectBtn, &QPushButton::clicked, [this, device]() {
        m_btManager->connectToDevice(device.address);
    });
}

void BluetoothPanel::onDevicesCleared() {
    for (auto* widget : m_deviceWidgets) {
        m_devicesLayout->removeWidget(widget);
        widget->deleteLater();
    }
    m_deviceWidgets.clear();
}

void BluetoothPanel::onScanningStarted() {
    m_refreshBtn->setEnabled(false);
}

void BluetoothPanel::onScanningStopped() {
    m_refreshBtn->setEnabled(true);
}

void BluetoothPanel::onConnecting() {
    SnackBar::show(window(), "Connecting to GMSync", SnackBar::Info);
    
    for (const QString& addr : m_deviceWidgets.keys()) {
        m_deviceWidgets[addr]->hide();
    }
}

void BluetoothPanel::onConnected(const QString& deviceName) {
    SnackBar::show(window(), "✓ Connected to GMSync", SnackBar::Success);
    
    onDevicesCleared();
    
    auto* connectedWidget = new QWidget(m_devicesContainer);
    connectedWidget->setStyleSheet(
        "QWidget { background: rgba(13, 19, 40, 204); border: 1px solid rgb(79, 209, 197); border-radius: 10px; }"
    );
    
    auto* layout = new QHBoxLayout(connectedWidget);
    layout->setContentsMargins(12, 10, 12, 10);
    
    auto* nameLabel = new QLabel(deviceName, connectedWidget);
    nameLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
    
    auto* disconnectBtn = new QPushButton("Disconnect", connectedWidget);
    disconnectBtn->setStyleSheet(
        "QPushButton { background: rgb(79, 209, 197); border: none; "
        "border-radius: 8px; color: rgb(10, 15, 25); padding: 6px 14px; font-size: 12px; font-weight: 600; }"
        "QPushButton:hover { background: rgb(65, 195, 183); }"
    );
    disconnectBtn->setCursor(Qt::PointingHandCursor);
    
    layout->addWidget(nameLabel);
    layout->addStretch();
    layout->addWidget(disconnectBtn);
    connectedWidget->setLayout(layout);
    
    m_devicesLayout->insertWidget(0, connectedWidget);
    m_deviceWidgets["__connected__"] = connectedWidget;
    
    connect(disconnectBtn, &QPushButton::clicked, [this]() {
        m_btManager->disconnectDevice();
    });
    
    emit connectionChanged(true);
}

void BluetoothPanel::onDisconnected() {
    SnackBar::show(window(), "Disconnected", SnackBar::Info);
    onDevicesCleared();
    emit connectionChanged(false);
}