#include "QRPanel.h"
#include "common/AppTheme.h"
#include "common/AppColors.h"

QRPanel::QRPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 14);
    layout->setSpacing(8);
    
    m_titleLabel = new QLabel("Pulse ID Check-In", this);
    m_titleLabel->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230, 233, 255);");
    
    m_hintLabel = new QLabel("Scan your wristband or ID card to auto-populate shooter data.", this);
    m_hintLabel->setStyleSheet("font-size: 12px; color: rgb(140, 147, 181);");
    m_hintLabel->setWordWrap(true);
    
    m_qrBox = new QWidget(this);
    m_qrBox->setFixedHeight(200);
    m_qrBox->setStyleSheet(
        "background: qradialgradient(cx: 0.5, cy: 0.5, radius: 0.8, "
        "fx: 0.5, fy: 0.5, "
        "stop: 0 rgba(255, 255, 255, 13), "
        "stop: 1 rgba(5, 7, 16, 242)); "
        "border: 1px dashed rgba(255, 255, 255, 46); "
        "border-radius: 12px;"
    );
    
    auto* qrLabel = new QLabel("SCAN ZONE", m_qrBox);
    qrLabel->setStyleSheet("font-size: 11px; color: rgb(163, 173, 212); letter-spacing: 2px;");
    qrLabel->setAlignment(Qt::AlignCenter);
    
    auto* qrLayout = new QVBoxLayout(m_qrBox);
    qrLayout->addWidget(qrLabel);
    m_qrBox->setLayout(qrLayout);
    
    m_footerLabel = new QLabel("or log in manually via <span style='color: rgb(79, 209, 197);'>Pulse ID</span>", this);
    m_footerLabel->setStyleSheet("font-size: 11px; color: rgb(113, 120, 164);");
    
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_qrBox);
    layout->addWidget(m_footerLabel);
    
    setLayout(layout);
    setStyleSheet(AppTheme::getPanelStyle());
}
