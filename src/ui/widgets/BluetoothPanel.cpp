#include "BluetoothPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"

// ── Constructor ───────────────────────────────────────────────────────────────

BluetoothPanel::BluetoothPanel(BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_btManager(btManager)
{
    buildUI();

    connect(m_btManager, &BluetoothManager::deviceFound,
            this, &BluetoothPanel::onDeviceFound);
    connect(m_btManager, &BluetoothManager::devicesCleared,
            this, &BluetoothPanel::onDevicesCleared);
    connect(m_btManager, &BluetoothManager::scanningStarted,
            this, &BluetoothPanel::onScanningStarted);
    connect(m_btManager, &BluetoothManager::scanningStopped,
            this, &BluetoothPanel::onScanningStopped);
    connect(m_btManager, &BluetoothManager::connecting,
            this, &BluetoothPanel::onConnecting);
    connect(m_btManager, &BluetoothManager::connected,
            this, &BluetoothPanel::onConnected);
    connect(m_btManager, &BluetoothManager::disconnected,
            this, &BluetoothPanel::onDisconnected);
    connect(m_btManager, &BluetoothManager::bluetoothPoweredChanged,
            this, &BluetoothPanel::onBluetoothPoweredChanged);

    onBluetoothPoweredChanged(m_btManager->isBluetoothPowered());

    if (m_btManager->isBluetoothPowered())
        m_btManager->startScanning();
}

// ── UI Construction ───────────────────────────────────────────────────────────

void BluetoothPanel::buildUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 18);
    layout->setSpacing(12);

    // ── Header row ────────────────────────────────────────────────────────
    m_headerRow = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(m_headerRow);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_headerLabel = new QLabel("Bluetooth Connections", m_headerRow);
    m_headerLabel->setStyleSheet(AppTheme::sectionTitle());

    m_refreshBtn = new QPushButton("↻", m_headerRow);
    m_refreshBtn->setStyleSheet(AppTheme::refreshButton());
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedSize(36, 36);

    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshBtn);
    m_headerRow->setLayout(headerLayout);

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

    auto* offHint = new QLabel(
        "Enable Bluetooth to discover and connect AimSync devices.", m_offlinePanel);
    offHint->setStyleSheet(AppTheme::labelMuted());
    offHint->setWordWrap(true);
    offHint->setAlignment(Qt::AlignCenter);

    m_bluetoothBtn = new QPushButton("Turn On Bluetooth", m_offlinePanel);
    m_bluetoothBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_bluetoothBtn->setCursor(Qt::PointingHandCursor);

    offLayout->addWidget(offIcon);
    offLayout->addWidget(offTitle);
    offLayout->addWidget(offHint);
    offLayout->addWidget(m_bluetoothBtn);
    m_offlinePanel->setLayout(offLayout);
    m_offlinePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // ── Help panel ────────────────────────────────────────────────────────
    m_helpPanel = new QWidget(this);
    m_helpPanel->setStyleSheet(AppTheme::helpPanel());
    auto* helpLayout = new QVBoxLayout(m_helpPanel);
    helpLayout->setContentsMargins(12, 10, 12, 10);
    helpLayout->setSpacing(4);

    auto* helpTitle = new QLabel("If device is not listed", m_helpPanel);
    helpTitle->setStyleSheet(AppTheme::helpTitle());

    auto* help1 = new QLabel("• Ensure the AimSync device is on.", m_helpPanel);
    help1->setStyleSheet(AppTheme::helpItem());
    auto* help2 = new QLabel("• Restart the AimSync device.", m_helpPanel);
    help2->setStyleSheet(AppTheme::helpItem());
    auto* help3 = new QLabel("• If issue persists, contact:", m_helpPanel);
    help3->setStyleSheet(AppTheme::helpItem());
    auto* helpEmail = new QLabel("  support@pulsestation.com", m_helpPanel);
    helpEmail->setStyleSheet(AppTheme::helpEmail());

    helpLayout->addWidget(helpTitle);
    helpLayout->addWidget(help1);
    helpLayout->addWidget(help2);
    helpLayout->addWidget(help3);
    helpLayout->addWidget(helpEmail);
    m_helpPanel->setLayout(helpLayout);

    // ── Device list scroll area ───────────────────────────────────────────
    m_devicesScroll = new QScrollArea(this);
    m_devicesScroll->setWidgetResizable(true);
    m_devicesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_devicesScroll->setStyleSheet(AppTheme::scrollArea());

    m_devicesContainer = new QWidget();
    m_devicesLayout = new QVBoxLayout(m_devicesContainer);
    m_devicesLayout->setContentsMargins(0, 0, 0, 0);
    m_devicesLayout->setSpacing(10);

    // Empty state
    m_emptyState = new QWidget(m_devicesContainer);
    m_emptyState->setStyleSheet(AppTheme::transparent());
    auto* emptyLayout = new QVBoxLayout(m_emptyState);
    emptyLayout->setContentsMargins(0, 24, 0, 24);
    emptyLayout->setAlignment(Qt::AlignCenter);

    auto* emptyIcon = new QLabel("◌", m_emptyState);
    emptyIcon->setStyleSheet(AppTheme::emptyIcon());
    emptyIcon->setAlignment(Qt::AlignCenter);

    auto* emptyText = new QLabel("No devices found", m_emptyState);
    emptyText->setStyleSheet(AppTheme::emptyText());
    emptyText->setAlignment(Qt::AlignCenter);

    emptyLayout->addWidget(emptyIcon);
    emptyLayout->addWidget(emptyText);
    m_emptyState->setLayout(emptyLayout);
    m_emptyState->hide();

    // Scan loader
    m_scanLoader = new QWidget(m_devicesContainer);
    m_scanLoader->setStyleSheet(AppTheme::transparent());
    auto* scanLayout = new QVBoxLayout(m_scanLoader);
    scanLayout->setContentsMargins(0, 24, 0, 24);
    scanLayout->setAlignment(Qt::AlignCenter);

    auto* scanDots = new QLabel("◉ ◎ ◎", m_scanLoader);
    scanDots->setObjectName("scanDots");
    scanDots->setStyleSheet(AppTheme::loaderDots());
    scanDots->setAlignment(Qt::AlignCenter);

    auto* scanText = new QLabel("Scanning for devices…", m_scanLoader);
    scanText->setStyleSheet(AppTheme::loaderText());
    scanText->setAlignment(Qt::AlignCenter);

    scanLayout->addWidget(scanDots);
    scanLayout->addWidget(scanText);
    m_scanLoader->setLayout(scanLayout);
    m_scanLoader->hide();

    // Connect loader
    m_connectLoader = new QWidget(m_devicesContainer);
    m_connectLoader->setStyleSheet(AppTheme::transparent());
    auto* connLayout = new QVBoxLayout(m_connectLoader);
    connLayout->setContentsMargins(18, 18, 18, 18);
    connLayout->setAlignment(Qt::AlignCenter);

    auto* connDots = new QLabel("◉ ◎ ◎", m_connectLoader);
    connDots->setObjectName("connDots");
    connDots->setStyleSheet(AppTheme::loaderDots());
    connDots->setAlignment(Qt::AlignCenter);

    auto* connText = new QLabel("Connecting…", m_connectLoader);
    connText->setObjectName("connText");
    connText->setStyleSheet(AppTheme::loaderText());
    connText->setAlignment(Qt::AlignCenter);

    connLayout->addWidget(connDots);
    connLayout->addWidget(connText);
    m_connectLoader->setLayout(connLayout);
    m_connectLoader->hide();

    // Assemble device list container
    m_devicesLayout->addWidget(m_connectLoader);
    m_devicesLayout->addWidget(m_scanLoader);
    m_devicesLayout->addWidget(m_emptyState);
    m_devicesLayout->addStretch();
    m_devicesContainer->setLayout(m_devicesLayout);
    m_devicesScroll->setWidget(m_devicesContainer);

    // ── Dot animator ──────────────────────────────────────────────────────
    m_dotTimer = new QTimer(this);
    m_dotTimer->setInterval(400);
    connect(m_dotTimer, &QTimer::timeout, [this]() {
        m_dotCount = (m_dotCount + 1) % 3;
        static const char* frames[] = {"◉ ◎ ◎", "◎ ◉ ◎", "◎ ◎ ◉"};
        if (auto* w = findChild<QLabel*>("scanDots")) w->setText(frames[m_dotCount]);
        if (auto* w = findChild<QLabel*>("connDots")) w->setText(frames[m_dotCount]);
    });

    // ── Connections ───────────────────────────────────────────────────────
    connect(m_refreshBtn, &QPushButton::clicked,
            [this]() { m_btManager->restartScanning(); });

    connect(m_bluetoothBtn, &QPushButton::clicked,
            [this]() { QProcess::startDetached("rfkill", {"unblock", "bluetooth"}); });

    // ── Assemble main layout ──────────────────────────────────────────────
    layout->addWidget(m_headerRow);
    layout->addWidget(m_offlinePanel);
    layout->addWidget(m_devicesScroll);
    layout->addWidget(m_helpPanel);
    setLayout(layout);
}

// ── Slot Implementations ──────────────────────────────────────────────────────

void BluetoothPanel::onDeviceFound(const BluetoothDevice& device) {
    // Only show whitelisted device name prefixes
    static const QStringList allowed = {"GMSync", "RA", "SK", "PulseStation"};
    const bool valid = std::any_of(allowed.begin(), allowed.end(),
        [&device](const QString& prefix) {
            return device.name.contains(prefix, Qt::CaseInsensitive);
        });
    if (!valid || m_deviceWidgets.contains(device.address)) return;

    auto* card = new QWidget(m_devicesContainer);
    card->setStyleSheet(AppTheme::deviceCard());

    auto* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(14, 12, 14, 12);

    auto* infoBlock = new QWidget(card);
    infoBlock->setStyleSheet(AppTheme::transparent());
    auto* infoLayout = new QVBoxLayout(infoBlock);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(3);

    auto* nameLabel = new QLabel(device.name, infoBlock);
    nameLabel->setStyleSheet(AppTheme::deviceName());

    auto* addrLabel = new QLabel(device.address, infoBlock);
    addrLabel->setStyleSheet(AppTheme::deviceAddress());

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(addrLabel);
    infoBlock->setLayout(infoLayout);

    auto* connectBtn = new QPushButton("Connect", card);
    connectBtn->setStyleSheet(AppTheme::connectButton());
    connectBtn->setCursor(Qt::PointingHandCursor);

    cardLayout->addWidget(infoBlock);
    cardLayout->addStretch();
    cardLayout->addWidget(connectBtn);
    card->setLayout(cardLayout);

    // Insert before the stretch at end
    m_devicesLayout->insertWidget(m_devicesLayout->count() - 1, card);
    m_deviceWidgets[device.address] = card;

    connect(connectBtn, &QPushButton::clicked,
            [this, device]() { m_btManager->connectToDevice(device.address); });
}

void BluetoothPanel::onDevicesCleared() {
    m_emptyState->hide();
    for (auto* w : m_deviceWidgets) {
        m_devicesLayout->removeWidget(w);
        w->deleteLater();
    }
    m_deviceWidgets.clear();
}

void BluetoothPanel::onScanningStarted() {
    m_refreshBtn->setEnabled(false);
    m_emptyState->hide();
    if (m_deviceWidgets.isEmpty()) {
        m_scanLoader->show();
        m_dotTimer->start();
    }
}

void BluetoothPanel::onScanningStopped() {
    m_refreshBtn->setEnabled(true);
    if (!m_connectLoader->isVisible()) {
        m_scanLoader->hide();
        m_dotTimer->stop();
        if (m_deviceWidgets.isEmpty())
            m_emptyState->show();
    }
}

void BluetoothPanel::onConnecting() {
    SnackBar::show(window(), "Connecting to device…", SnackBar::Info);
    m_scanLoader->hide();
    for (auto* w : m_deviceWidgets) w->hide();
    if (auto* t = m_connectLoader->findChild<QLabel*>("connText"))
        t->setText("Connecting…");
    m_connectLoader->show();
    m_dotTimer->start();
}

void BluetoothPanel::onConnected(const QString& deviceName) {
    m_connectLoader->hide();
    m_dotTimer->stop();
    SnackBar::show(window(), QString("✓ Connected to %1").arg(deviceName), SnackBar::Success);

    onDevicesCleared();

    auto* card = new QWidget(m_devicesContainer);
    card->setStyleSheet(AppTheme::connectedCard());

    auto* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(14, 12, 14, 12);

    auto* nameLabel = new QLabel(deviceName, card);
    nameLabel->setStyleSheet(AppTheme::deviceName());

    auto* disconnectBtn = new QPushButton("Disconnect", card);
    disconnectBtn->setStyleSheet(AppTheme::disconnectButton());
    disconnectBtn->setCursor(Qt::PointingHandCursor);

    cardLayout->addWidget(nameLabel);
    cardLayout->addStretch();
    cardLayout->addWidget(disconnectBtn);
    card->setLayout(cardLayout);

    m_devicesLayout->insertWidget(0, card);
    m_deviceWidgets["__connected__"] = card;

    connect(disconnectBtn, &QPushButton::clicked, [this]() {
        onDevicesCleared();
        if (auto* t = m_connectLoader->findChild<QLabel*>("connText"))
            t->setText("Disconnecting…");
        m_connectLoader->show();
        m_dotTimer->start();
        m_btManager->disconnectDevice();
    });

    emit connectionChanged(true);
}

void BluetoothPanel::onDisconnected() {
    m_connectLoader->hide();
    m_dotTimer->stop();
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
