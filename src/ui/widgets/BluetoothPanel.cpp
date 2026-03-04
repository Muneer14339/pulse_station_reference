#include "BluetoothPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QLabel>
#include <QHBoxLayout>

BluetoothPanel::BluetoothPanel(BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_btManager(btManager)
{
    using namespace AppTheme;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // ── Offline panel ─────────────────────────────────────────────────────
    m_offlinePanel = new QWidget(this);
    m_offlinePanel->setAttribute(Qt::WA_StyledBackground, true);
    m_offlinePanel->setStyleSheet(AppTheme::offlinePanel());
    auto* offLayout = new QVBoxLayout(m_offlinePanel);
    offLayout->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, PanelPadV);
    offLayout->setSpacing(ItemGap);
    offLayout->setAlignment(Qt::AlignTop);

    auto* offIcon = new QLabel("\u26A0", m_offlinePanel);   // ⚠
    offIcon->setStyleSheet(AppTheme::warningIcon());
    offIcon->setAlignment(Qt::AlignCenter);
    auto* offTitle = new QLabel("Bluetooth is Off", m_offlinePanel);
    offTitle->setStyleSheet(AppTheme::sectionTitle());
    offTitle->setAlignment(Qt::AlignCenter);
    auto* offHint = new QLabel(
        "Enable Bluetooth to discover AimSync devices.", m_offlinePanel);
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
    offLayout->addSpacing(SpaceXS);
    offLayout->addWidget(btBtn);
    m_offlinePanel->setLayout(offLayout);

    // ── ScanPanel ─────────────────────────────────────────────────────────
    m_scanPanel = new ScanPanel("Bluetooth Connections", this);

    // ── Help panel (wrapped so outer margins match ScanPanel's internal padding) ─
    m_helpPanel = new QWidget(this);
    m_helpPanel->setAttribute(Qt::WA_StyledBackground, true);
    m_helpPanel->setStyleSheet(AppTheme::helpPanel());
    auto* helpLayout = new QVBoxLayout(m_helpPanel);
    helpLayout->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, PanelPadV);
    helpLayout->setSpacing(SpaceXS);

    auto addHelp = [&](const QString& text, const QString& style) {
        auto* l = new QLabel(text, m_helpPanel);
        l->setStyleSheet(style);
        l->setWordWrap(true);   // prevent right-side clip on long lines
        helpLayout->addWidget(l);
    };
    addHelp("If device is not listed", AppTheme::helpTitle());
    addHelp("\u2022 Ensure the AimSync device is on.", AppTheme::helpItem());
    addHelp("\u2022 Restart the AimSync device.", AppTheme::helpItem());
    addHelp("\u2022 If issue persists: support@pulsestation.com", AppTheme::helpEmail());
    m_helpPanel->setLayout(helpLayout);

    // Wrapper gives the help card the same left/right/bottom outer gap as
    // the ScanPanel's internal padding — keeps both visually balanced.
    // ObjectName lets onBluetoothPoweredChanged toggle it without a member pointer.
    auto* helpWrapper = new QWidget(this);
    helpWrapper->setObjectName("helpWrapper");
    helpWrapper->setStyleSheet(AppTheme::transparent());
    auto* hwl = new QVBoxLayout(helpWrapper);
    hwl->setContentsMargins(PanelPadH, 0, PanelPadH, PanelPadV);
    hwl->setSpacing(0);
    hwl->addWidget(m_helpPanel);
    helpWrapper->setLayout(hwl);

    layout->addWidget(m_offlinePanel);
    layout->addWidget(m_scanPanel, 1);
    layout->addWidget(helpWrapper);
    setLayout(layout);

    // ── Wire signals ──────────────────────────────────────────────────────
    connect(m_scanPanel, &ScanPanel::refreshClicked,
            m_btManager, &BluetoothManager::restartScanning);
    connect(m_scanPanel, &ScanPanel::connectClicked,
            m_btManager, &BluetoothManager::connectToDevice);
    connect(m_scanPanel, &ScanPanel::disconnectClicked,
            [this]() {
                m_scanPanel->showConnecting("Disconnecting\u2026");
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
    static const QStringList allowed = {"GMSync", "RA", "SK", "ShotPulse"};
    const bool valid = std::any_of(allowed.begin(), allowed.end(),
        [&](const QString& p) { return device.name.contains(p, Qt::CaseInsensitive); });
    if (!valid) return;
    m_scanPanel->addDevice(device.address, device.name, device.address);
}

void BluetoothPanel::onDevicesCleared()   { m_scanPanel->clearDevices(); }
void BluetoothPanel::onScanningStarted()  { m_scanPanel->startScanning(); }
void BluetoothPanel::onScanningStopped()  { m_scanPanel->stopScanning(); }

void BluetoothPanel::onConnecting() {
    SnackBar::show(window(), "Connecting to device\u2026", SnackBar::Info);
    m_scanPanel->showConnecting();
}

void BluetoothPanel::onConnected(const QString& name) {
    SnackBar::show(window(), QString("\u2713 Connected to %1").arg(name), SnackBar::Success);
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
    // Toggle the whole wrapper (which contains m_helpPanel + outer margins)
    if (auto* hw = findChild<QWidget*>("helpWrapper"))
        hw->setVisible(powered);
    m_helpPanel->setVisible(powered);
    if (powered)
        SnackBar::show(window(), "Bluetooth enabled", SnackBar::Success);
    else
        SnackBar::show(window(), "Bluetooth is off", SnackBar::Error);
}