#include "BluetoothPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QLabel>
#include <QHBoxLayout>

BluetoothPanel::BluetoothPanel(BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_btManager(btManager)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // ── Offline panel ─────────────────────────────────────────────────────
    m_offlinePanel = new QWidget(this);
    m_offlinePanel->setStyleSheet(AppTheme::offlinePanel());
    auto* offLayout = new QVBoxLayout(m_offlinePanel);
    offLayout->setContentsMargins(18, 18, 18, 18);
    offLayout->setSpacing(12);

    auto* offIcon = new QLabel("⚠", m_offlinePanel);
    offIcon->setStyleSheet(AppTheme::warningIcon());
    offIcon->setAlignment(Qt::AlignCenter);
    auto* offTitle = new QLabel("Bluetooth is Off", m_offlinePanel);
    offTitle->setStyleSheet(AppTheme::sectionTitle());
    offTitle->setAlignment(Qt::AlignCenter);
    auto* offHint = new QLabel("Enable Bluetooth to discover AimSync devices.", m_offlinePanel);
    offHint->setStyleSheet(AppTheme::labelMuted());
    offHint->setWordWrap(true);
    offHint->setAlignment(Qt::AlignCenter);
    auto* btBtn = new QPushButton("Turn On Bluetooth", m_offlinePanel);
    btBtn->setStyleSheet(AppTheme::buttonPrimary());
    btBtn->setCursor(Qt::PointingHandCursor);
    connect(btBtn, &QPushButton::clicked,
            []() { QProcess::startDetached("rfkill", {"unblock", "bluetooth"}); });

    offLayout->addWidget(offIcon);
    offLayout->addWidget(offTitle);
    offLayout->addWidget(offHint);
    offLayout->addWidget(btBtn);
    m_offlinePanel->setLayout(offLayout);

    // ── ScanPanel ─────────────────────────────────────────────────────────
    m_scanPanel = new ScanPanel("Bluetooth Connections", this);

    // ── Help panel ────────────────────────────────────────────────────────
    m_helpPanel = new QWidget(this);
    m_helpPanel->setStyleSheet(AppTheme::helpPanel());
    auto* helpLayout = new QVBoxLayout(m_helpPanel);
    helpLayout->setContentsMargins(12, 10, 12, 10);
    helpLayout->setSpacing(4);
    auto addHelp = [&](const QString& text, const QString& style) {
        auto* l = new QLabel(text, m_helpPanel);
        l->setStyleSheet(style);
        helpLayout->addWidget(l);
    };
    addHelp("If device is not listed", AppTheme::helpTitle());
    addHelp("• Ensure the AimSync device is on.", AppTheme::helpItem());
    addHelp("• Restart the AimSync device.", AppTheme::helpItem());
    addHelp("• If issue persists: support@pulsestation.com", AppTheme::helpEmail());
    m_helpPanel->setLayout(helpLayout);

    layout->addWidget(m_offlinePanel);
    layout->addWidget(m_scanPanel, 1);
    layout->addWidget(m_helpPanel);
    setLayout(layout);

    // ── Wire signals ──────────────────────────────────────────────────────
    connect(m_scanPanel, &ScanPanel::refreshClicked,
            m_btManager, &BluetoothManager::restartScanning);
    connect(m_scanPanel, &ScanPanel::connectClicked,
            m_btManager, &BluetoothManager::connectToDevice);
    connect(m_scanPanel, &ScanPanel::disconnectClicked,
            [this]() {
                m_scanPanel->showConnecting("Disconnecting…");
                m_btManager->disconnectDevice();
            });

    connect(m_btManager, &BluetoothManager::deviceFound,    this, &BluetoothPanel::onDeviceFound);
    connect(m_btManager, &BluetoothManager::devicesCleared, this, &BluetoothPanel::onDevicesCleared);
    connect(m_btManager, &BluetoothManager::scanningStarted,this, &BluetoothPanel::onScanningStarted);
    connect(m_btManager, &BluetoothManager::scanningStopped,this, &BluetoothPanel::onScanningStopped);
    connect(m_btManager, &BluetoothManager::connecting,     this, &BluetoothPanel::onConnecting);
    connect(m_btManager, &BluetoothManager::connected,      this, &BluetoothPanel::onConnected);
    connect(m_btManager, &BluetoothManager::disconnected,   this, &BluetoothPanel::onDisconnected);
    connect(m_btManager, &BluetoothManager::bluetoothPoweredChanged,
            this, &BluetoothPanel::onBluetoothPoweredChanged);

    onBluetoothPoweredChanged(m_btManager->isBluetoothPowered());
    if (m_btManager->isBluetoothPowered())
        m_btManager->startScanning();
}

void BluetoothPanel::onDeviceFound(const BluetoothDevice& device) {
    static const QStringList allowed = {"GMSync", "RA", "SK", "PulseStation"};
    const bool valid = std::any_of(allowed.begin(), allowed.end(),
        [&](const QString& p) { return device.name.contains(p, Qt::CaseInsensitive); });
    if (!valid) return;
    m_scanPanel->addDevice(device.address, device.name, device.address);
}

void BluetoothPanel::onDevicesCleared()   { m_scanPanel->clearDevices(); }
void BluetoothPanel::onScanningStarted()  { m_scanPanel->startScanning(); }
void BluetoothPanel::onScanningStopped()  { m_scanPanel->stopScanning(); }

void BluetoothPanel::onConnecting() {
    SnackBar::show(window(), "Connecting to device…", SnackBar::Info);
    m_scanPanel->showConnecting();
}

void BluetoothPanel::onConnected(const QString& name) {
    SnackBar::show(window(), QString("✓ Connected to %1").arg(name), SnackBar::Success);
    m_scanPanel->showConnected(name);
    emit connectionChanged(true);
}

void BluetoothPanel::onDisconnected() {
    SnackBar::show(window(), "Disconnected", SnackBar::Info);
    m_scanPanel->showDisconnected();
    emit connectionChanged(false);
}

void BluetoothPanel::onBluetoothPoweredChanged(bool powered) {
    m_offlinePanel->setVisible(!powered);
    m_scanPanel->setVisible(powered);
    m_helpPanel->setVisible(powered);
    if (powered)
        SnackBar::show(window(), "Bluetooth enabled", SnackBar::Success);
    else
        SnackBar::show(window(), "Bluetooth is off", SnackBar::Error);
}