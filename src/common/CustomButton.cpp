#include "CustomButton.h"
#include "AppTheme.h"
#include <QVBoxLayout>

CustomButton::CustomButton(const QString& mainText, const QString& subText, QWidget* parent)
    : QPushButton(parent), m_selected(false) {
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(2);
    
    m_mainLabel = new QLabel(mainText, this);
    m_mainLabel->setStyleSheet("font-weight: 600; font-size: 14px; color: rgb(245, 245, 255); background: transparent; border: none;");
    m_mainLabel->setWordWrap(true);
    
    m_subLabel = new QLabel(subText, this);
    m_subLabel->setStyleSheet("font-size: 11px; color: rgb(165, 172, 201); background: transparent; border: none;");
    m_subLabel->setWordWrap(true);
    
    layout->addWidget(m_mainLabel);
    if (!subText.isEmpty()) {
        layout->addWidget(m_subLabel);
    }
    
    setLayout(layout);
    updateStyle();
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(60);
}

void CustomButton::setMainText(const QString& text) {
    m_mainLabel->setText(text);
}

void CustomButton::setSubText(const QString& text) {
    m_subLabel->setText(text);
    m_subLabel->setVisible(!text.isEmpty());
}

void CustomButton::setSelected(bool selected) {
    m_selected = selected;
    updateStyle();
}

void CustomButton::enterEvent(QEnterEvent* event) {
    if (!m_selected) {
        setStyleSheet(AppTheme::getButtonHoverStyle());
    }
    QPushButton::enterEvent(event);
}

void CustomButton::leaveEvent(QEvent* event) {
    updateStyle();
    QPushButton::leaveEvent(event);
}

void CustomButton::updateStyle() {
    setStyleSheet(m_selected ? AppTheme::getButtonSelectedStyle() : AppTheme::getButtonStyle());
}