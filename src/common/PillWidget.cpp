#include "PillWidget.h"
#include "AppTheme.h"

PillWidget::PillWidget(const QString& text, QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 12, 5);
    layout->setSpacing(7);

    m_dot = new QWidget(this);
    m_dot->setFixedSize(8, 8);

    m_label = new QLabel(text, this);

    layout->addWidget(m_dot);
    layout->addWidget(m_label);
    setLayout(layout);

    applyStyle();
}

void PillWidget::setActive(bool active) {
    m_active = active;
    applyStyle();
}

void PillWidget::applyStyle() {
    if (m_active) {
        setStyleSheet(AppTheme::pillActive());
        m_dot->setStyleSheet(AppTheme::pillDotActive());
        m_label->setStyleSheet(AppTheme::pillLabelActive());
    } else {
        setStyleSheet(AppTheme::pillInactive());
        m_dot->setStyleSheet(AppTheme::pillDotInactive());
        m_label->setStyleSheet(AppTheme::pillLabelInactive());
    }
}
