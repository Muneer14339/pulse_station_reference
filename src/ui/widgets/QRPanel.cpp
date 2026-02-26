#include "QRPanel.h"
#include "common/AppTheme.h"

QRPanel::QRPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 16);
    layout->setSpacing(10);

    m_titleLabel = new QLabel("Pulse ID Check-In", this);
    m_titleLabel->setStyleSheet(AppTheme::sectionTitle());

    m_hintLabel = new QLabel("Scan your wristband or ID card to auto-populate shooter data.", this);
    m_hintLabel->setStyleSheet(AppTheme::labelMuted());
    m_hintLabel->setWordWrap(true);

    m_scanZone = new QWidget(this);
    m_scanZone->setFixedHeight(200);
    m_scanZone->setStyleSheet(AppTheme::qrScanZone());

    auto* scanLabel = new QLabel("SCAN ZONE", m_scanZone);
    scanLabel->setStyleSheet(AppTheme::qrScanLabel());
    scanLabel->setAlignment(Qt::AlignCenter);

    auto* scanLayout = new QVBoxLayout(m_scanZone);
    scanLayout->addWidget(scanLabel);
    m_scanZone->setLayout(scanLayout);

    m_footerLabel = new QLabel(
        "or log in manually via <span style='color: rgb(79,209,197);'>Pulse ID</span>", this);
    m_footerLabel->setStyleSheet(AppTheme::labelSubtle());

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_scanZone);
    layout->addWidget(m_footerLabel);
    setLayout(layout);

    setStyleSheet(AppTheme::panel());
}
