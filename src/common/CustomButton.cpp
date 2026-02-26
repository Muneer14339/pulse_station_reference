#include "CustomButton.h"
#include "AppTheme.h"

CustomButton::CustomButton(const QString& mainText, const QString& subText, QWidget* parent)
    : QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
    setMinimumHeight(70);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(4);

    m_mainLabel = new QLabel(mainText, this);
    m_mainLabel->setStyleSheet(AppTheme::customButtonMain());
    m_mainLabel->setWordWrap(true);

    m_subLabel = new QLabel(subText, this);
    m_subLabel->setStyleSheet(AppTheme::customButtonSub());
    m_subLabel->setWordWrap(true);
    m_subLabel->setVisible(!subText.isEmpty());

    layout->addWidget(m_mainLabel);
    layout->addWidget(m_subLabel);
    setLayout(layout);

    applyStyle();
}

void CustomButton::setMainText(const QString& text) { m_mainLabel->setText(text); }

void CustomButton::setSubText(const QString& text) {
    m_subLabel->setText(text);
    m_subLabel->setVisible(!text.isEmpty());
}

void CustomButton::setSelected(bool selected) {
    m_selected = selected;
    applyStyle();
}

void CustomButton::enterEvent(QEvent* event) {
    if (!m_selected)
        setStyleSheet(AppTheme::buttonHover());
    QPushButton::enterEvent(event);
}

void CustomButton::leaveEvent(QEvent* event) {
    applyStyle();
    QPushButton::leaveEvent(event);
}

void CustomButton::applyStyle() {
    setStyleSheet(m_selected ? AppTheme::buttonSelected() : AppTheme::buttonNormal());
}
