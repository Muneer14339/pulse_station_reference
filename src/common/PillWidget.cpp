#include "PillWidget.h"
#include "AppTheme.h"

PillWidget::PillWidget(const QString& text, QWidget* parent) : QWidget(parent) {
    using namespace AppTheme;

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(SpaceM, SpaceXS, SpaceL, SpaceXS);
    layout->setSpacing(InlineGap);

    m_dot = new QWidget(this);
    m_dot->setFixedSize(SpaceS, SpaceS);

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