#include "ScanPanel.h"
#include "AppTheme.h"

ScanPanel::ScanPanel(const QString& title, QWidget* parent) : QWidget(parent) {
    buildUI(title);
}

void ScanPanel::buildUI(const QString& title) {
    using namespace AppTheme;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, PanelPadV);
    layout->setSpacing(ItemGap);

    // ── Header ────────────────────────────────────────────────────────────
    auto* headerRow = new QWidget(this);
    auto* headerLayout = new QHBoxLayout(headerRow);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(InlineGap);

    auto* titleLabel = new QLabel(title, headerRow);
    titleLabel->setStyleSheet(AppTheme::sectionTitle());
    titleLabel->setWordWrap(true);   // wraps "Bluetooth Connections" on narrow sidebar

    m_refreshBtn = new QPushButton("\u21BB", headerRow);   // ↻
    m_refreshBtn->setStyleSheet(AppTheme::iconButton());
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    m_refreshBtn->setFixedSize(IconBtnSz, IconBtnSz);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshBtn);
    headerRow->setLayout(headerLayout);

    // ── Device scroll area ────────────────────────────────────────────────
    m_devicesScroll = new QScrollArea(this);
    m_devicesScroll->setWidgetResizable(true);
    m_devicesScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_devicesScroll->setStyleSheet(AppTheme::scrollArea());

    m_devicesContainer = new QWidget();
    m_devicesLayout = new QVBoxLayout(m_devicesContainer);
    m_devicesLayout->setContentsMargins(0, 0, 0, 0);
    m_devicesLayout->setSpacing(ItemGap);

    // ── Scan loader ───────────────────────────────────────────────────────
    m_scanLoader = new QWidget(m_devicesContainer);
    m_scanLoader->setStyleSheet(AppTheme::transparent());
    auto* scanLayout = new QVBoxLayout(m_scanLoader);
    scanLayout->setContentsMargins(0, SpaceXXL, 0, SpaceXXL);
    scanLayout->setSpacing(InlineGap);
    scanLayout->setAlignment(Qt::AlignCenter);
    auto* scanDots = new QLabel("\u25C9 \u25CE \u25CE", m_scanLoader);
    scanDots->setObjectName("scanDots");
    scanDots->setStyleSheet(AppTheme::loaderDots());
    scanDots->setAlignment(Qt::AlignCenter);
    auto* scanText = new QLabel("Scanning\u2026", m_scanLoader);
    scanText->setStyleSheet(AppTheme::loaderText());
    scanText->setAlignment(Qt::AlignCenter);
    scanLayout->addWidget(scanDots);
    scanLayout->addWidget(scanText);
    m_scanLoader->setLayout(scanLayout);
    m_scanLoader->hide();

    // ── Connect loader ────────────────────────────────────────────────────
    m_connectLoader = new QWidget(m_devicesContainer);
    m_connectLoader->setStyleSheet(AppTheme::transparent());
    auto* connLayout = new QVBoxLayout(m_connectLoader);
    connLayout->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, PanelPadV);
    connLayout->setSpacing(InlineGap);
    connLayout->setAlignment(Qt::AlignCenter);
    auto* connDots = new QLabel("\u25C9 \u25CE \u25CE", m_connectLoader);
    connDots->setObjectName("connDots");
    connDots->setStyleSheet(AppTheme::loaderDots());
    connDots->setAlignment(Qt::AlignCenter);
    auto* connText = new QLabel("Connecting\u2026", m_connectLoader);
    connText->setObjectName("connText");
    connText->setStyleSheet(AppTheme::loaderText());
    connText->setAlignment(Qt::AlignCenter);
    connLayout->addWidget(connDots);
    connLayout->addWidget(connText);
    m_connectLoader->setLayout(connLayout);
    m_connectLoader->hide();

    // ── Empty state ───────────────────────────────────────────────────────
    m_emptyState = new QWidget(m_devicesContainer);
    m_emptyState->setStyleSheet(AppTheme::transparent());
    auto* emptyLayout = new QVBoxLayout(m_emptyState);
    emptyLayout->setContentsMargins(0, SpaceXXL, 0, SpaceXXL);
    emptyLayout->setSpacing(InlineGap);
    emptyLayout->setAlignment(Qt::AlignCenter);
    auto* emptyIcon = new QLabel("\u25CC", m_emptyState);
    emptyIcon->setStyleSheet(AppTheme::emptyIcon());
    emptyIcon->setAlignment(Qt::AlignCenter);
    auto* emptyText = new QLabel("Nothing found", m_emptyState);
    emptyText->setStyleSheet(AppTheme::emptyText());
    emptyText->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(emptyIcon);
    emptyLayout->addWidget(emptyText);
    m_emptyState->setLayout(emptyLayout);
    m_emptyState->hide();

    m_devicesLayout->addWidget(m_connectLoader);
    m_devicesLayout->addWidget(m_scanLoader);
    m_devicesLayout->addWidget(m_emptyState);
    m_devicesLayout->addStretch();
    m_devicesContainer->setLayout(m_devicesLayout);
    m_devicesScroll->setWidget(m_devicesContainer);

    // ── Dot animator ──────────────────────────────────────────────────────
    m_dotTimer = new QTimer(this);
    m_dotTimer->setInterval(400);
    connect(m_dotTimer, &QTimer::timeout, this, &ScanPanel::tickDots);
    connect(m_refreshBtn, &QPushButton::clicked, this, &ScanPanel::refreshClicked);

    layout->addWidget(headerRow);
    layout->addWidget(m_devicesScroll);
    setLayout(layout);
}

void ScanPanel::tickDots() {
    m_dotCount = (m_dotCount + 1) % 3;
    static const char* frames[] = {"\u25C9 \u25CE \u25CE",
                                    "\u25CE \u25C9 \u25CE",
                                    "\u25CE \u25CE \u25C9"};
    if (auto* w = findChild<QLabel*>("scanDots")) w->setText(frames[m_dotCount]);
    if (auto* w = findChild<QLabel*>("connDots")) w->setText(frames[m_dotCount]);
}

void ScanPanel::startScanning() {
    m_refreshBtn->setEnabled(false);
    m_emptyState->hide();
    if (m_deviceWidgets.isEmpty()) {
        m_scanLoader->show();
        m_dotTimer->start();
    }
}

void ScanPanel::stopScanning() {
    m_refreshBtn->setEnabled(true);
    if (!m_connectLoader->isVisible()) {
        m_scanLoader->hide();
        m_dotTimer->stop();
        if (m_deviceWidgets.isEmpty())
            m_emptyState->show();
    }
}

void ScanPanel::addDevice(const QString& id, const QString& primary, const QString& secondary) {
    if (m_deviceWidgets.contains(id)) return;

    using namespace AppTheme;

    auto* card = new QWidget(m_devicesContainer);
    card->setStyleSheet(AppTheme::deviceCard());
    auto* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(CardPadH, CardPadV, CardPadH, CardPadV);
    cardLayout->setSpacing(InlineGap);

    auto* infoBlock = new QWidget(card);
    infoBlock->setStyleSheet(AppTheme::transparent());
    auto* infoLayout = new QVBoxLayout(infoBlock);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(RowPad);
    auto* nameLabel = new QLabel(primary, infoBlock);
    nameLabel->setStyleSheet(AppTheme::deviceName());
    nameLabel->setWordWrap(true);
    auto* subLabel = new QLabel(secondary, infoBlock);
    subLabel->setStyleSheet(AppTheme::deviceAddress());
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(subLabel);
    infoBlock->setLayout(infoLayout);

    auto* connectBtn = new QPushButton("Connect", card);
    connectBtn->setStyleSheet(AppTheme::connectButton());
    connectBtn->setMinimumWidth(80);    // never clips on any reasonable sidebar width
    connectBtn->setCursor(Qt::PointingHandCursor);
    connect(connectBtn, &QPushButton::clicked, [this, id]() { emit connectClicked(id); });

    cardLayout->addWidget(infoBlock);
    cardLayout->addStretch();
    cardLayout->addWidget(connectBtn);
    card->setLayout(cardLayout);

    m_devicesLayout->insertWidget(m_devicesLayout->count() - 1, card);
    m_deviceWidgets[id] = card;
    m_scanLoader->hide();
    m_emptyState->hide();
}

void ScanPanel::clearDevices() {
    m_emptyState->hide();
    for (auto* w : m_deviceWidgets) {
        m_devicesLayout->removeWidget(w);
        w->deleteLater();
    }
    m_deviceWidgets.clear();
}

void ScanPanel::showConnecting(const QString& msg) {
    m_scanLoader->hide();
    for (auto* w : m_deviceWidgets) w->hide();
    if (auto* t = m_connectLoader->findChild<QLabel*>("connText")) t->setText(msg);
    m_connectLoader->show();
    m_dotTimer->start();
}

void ScanPanel::showConnected(const QString& name) {
    m_connectLoader->hide();
    m_dotTimer->stop();
    clearDevices();

    using namespace AppTheme;

    auto* card = new QWidget(m_devicesContainer);
    card->setStyleSheet(AppTheme::connectedCard());
    auto* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(CardPadH, CardPadV, CardPadH, CardPadV);
    cardLayout->setSpacing(InlineGap);

    auto* nameLabel = new QLabel(name, card);
    nameLabel->setStyleSheet(AppTheme::deviceName());
    nameLabel->setWordWrap(true);

    auto* disconnectBtn = new QPushButton("Disconnect", card);
    disconnectBtn->setStyleSheet(AppTheme::disconnectButton());
    disconnectBtn->setCursor(Qt::PointingHandCursor);
    connect(disconnectBtn, &QPushButton::clicked, this, &ScanPanel::disconnectClicked);

    cardLayout->addWidget(nameLabel);
    cardLayout->addStretch();
    cardLayout->addWidget(disconnectBtn);
    card->setLayout(cardLayout);

    m_devicesLayout->insertWidget(0, card);
    m_deviceWidgets["__connected__"] = card;
}

void ScanPanel::showDisconnected() {
    m_connectLoader->hide();
    m_dotTimer->stop();
    clearDevices();
}

void ScanPanel::setContentVisible(bool visible) {
    m_devicesScroll->setVisible(visible);
    m_refreshBtn->setVisible(visible);
}