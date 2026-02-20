#include "BluetoothPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"


BluetoothPanel::BluetoothPanel(BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_btManager(btManager) {
    setupUI();

    connect(m_btManager, &BluetoothManager::deviceFound,     this, &BluetoothPanel::onDeviceFound);
    connect(m_btManager, &BluetoothManager::devicesCleared,  this, &BluetoothPanel::onDevicesCleared);
    connect(m_btManager, &BluetoothManager::scanningStarted, this, &BluetoothPanel::onScanningStarted);
    connect(m_btManager, &BluetoothManager::scanningStopped, this, &BluetoothPanel::onScanningStopped);
    connect(m_btManager, &BluetoothManager::connecting,      this, &BluetoothPanel::onConnecting);
    connect(m_btManager, &BluetoothManager::connected,       this, &BluetoothPanel::onConnected);
    connect(m_btManager, &BluetoothManager::disconnected,    this, &BluetoothPanel::onDisconnected);
    connect(m_btManager, &BluetoothManager::bluetoothPoweredChanged, this, &BluetoothPanel::onBluetoothPoweredChanged);

    onBluetoothPoweredChanged(m_btManager->isBluetoothPowered());
    
    if (m_btManager->isBluetoothPowered()) m_btManager->startScanning();
}

void BluetoothPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 16);
    layout->setSpacing(10);

    // ── Header row ────────────────────────────────────────────────────────────
    m_headerRow = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(m_headerRow);;
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_headerLabel = new QLabel("Bluetooth Connections", m_headerRow);
    m_headerLabel->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");

    m_refreshBtn = new QPushButton("↻", m_headerRow);
    m_refreshBtn->setStyleSheet(
        "QPushButton { background: transparent; border: 1px solid rgba(79, 209, 197, 128); "
        "border-radius: 6px; color: rgb(79, 209, 197); padding: 4px 8px; font-size: 16px; }"
        "QPushButton:hover { background: rgba(79, 209, 197, 26); }"
        "QPushButton:disabled { color: rgba(79, 209, 197, 60); border-color: rgba(79, 209, 197, 40); }"
    );
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedSize(32, 32);

    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshBtn);
    m_headerRow->setLayout(headerLayout);

    // ── Bluetooth ON badge ────────────────────────────────────────────────────
    // ── Offline panel (shown when BT is off) ─────────────────────────────────
m_offlinePanel = new QWidget(this);
m_offlinePanel->setStyleSheet(AppTheme::getPanelStyle());
auto* offLayout = new QVBoxLayout(m_offlinePanel);
offLayout->setContentsMargins(16, 16, 16, 16);
offLayout->setSpacing(10);

auto* offIcon = new QLabel("⚠", m_offlinePanel);
offIcon->setStyleSheet("font-size: 22px; color: rgb(255,182,73); background: transparent; border: none;");
offIcon->setAlignment(Qt::AlignCenter);

auto* offTitle = new QLabel("Bluetooth is Off", m_offlinePanel);
offTitle->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230,233,255); background: transparent; border: none;");
offTitle->setAlignment(Qt::AlignCenter);

auto* offHint = new QLabel("Enable Bluetooth to discover and connect AimSync devices.", m_offlinePanel);
offHint->setStyleSheet("font-size: 11px; color: rgb(140,147,181); background: transparent; border: none;");
offHint->setWordWrap(true);
offHint->setAlignment(Qt::AlignCenter);

m_bluetoothBtn = new QPushButton("Turn On Bluetooth", m_offlinePanel);
m_bluetoothBtn->setStyleSheet(AppTheme::getButtonPrimaryStyle());
m_bluetoothBtn->setCursor(Qt::PointingHandCursor);

offLayout->addWidget(offIcon);
offLayout->addWidget(offTitle);
offLayout->addWidget(offHint);
offLayout->addWidget(m_bluetoothBtn);
m_offlinePanel->setLayout(offLayout);
m_offlinePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // ── Help panel ────────────────────────────────────────────────────────────
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

    // ── Device list ───────────────────────────────────────────────────────────
    m_devicesScroll = new QScrollArea(this);
    m_devicesScroll->setWidgetResizable(true);
    m_devicesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_devicesScroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    m_devicesContainer = new QWidget();
    m_devicesLayout    = new QVBoxLayout(m_devicesContainer);
    m_devicesLayout->setContentsMargins(0, 0, 0, 0);
    m_devicesLayout->setSpacing(8);
    m_devicesLayout->addStretch();
    m_devicesContainer->setLayout(m_devicesLayout);
    m_devicesScroll->setWidget(m_devicesContainer);

    layout->addWidget(m_headerRow);
layout->addWidget(m_offlinePanel);
layout->addWidget(m_devicesScroll);
layout->addWidget(m_helpPanel);
    setLayout(layout);

    // Refresh: force-clear + fresh scan regardless of current state
    connect(m_refreshBtn, &QPushButton::clicked, [this]() {
        m_btManager->restartScanning();
    });

    connect(m_bluetoothBtn, &QPushButton::clicked, [this]() {
    QProcess::startDetached("rfkill", {"unblock", "bluetooth"});
});
    
}

// ── Slot implementations ──────────────────────────────────────────────────────

void BluetoothPanel::onDeviceFound(const BluetoothDevice& device) {
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
    SnackBar::show(window(), "Connecting to device…", SnackBar::Info);
    for (auto* w : m_deviceWidgets) w->hide();
}

void BluetoothPanel::onConnected(const QString& deviceName) {
    SnackBar::show(window(), QString("✓ Connected to %1").arg(deviceName), SnackBar::Success);

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

void BluetoothPanel::onBluetoothPoweredChanged(bool powered) {
    m_headerRow->setVisible(powered);
    m_offlinePanel->setVisible(!powered);
    m_refreshBtn->setVisible(powered);
    m_devicesScroll->setVisible(powered);
    m_helpPanel->setVisible(powered);

    if (powered)
        SnackBar::show(window(), "Bluetooth enabled", SnackBar::Success);
    else
        SnackBar::show(window(), "Bluetooth is off", SnackBar::Error);
}