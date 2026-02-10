#include "PillWidget.h"
#include "AppColors.h"

PillWidget::PillWidget(const QString& text, QWidget* parent)
    : QWidget(parent), m_active(false) {
    
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(6);
    
    m_dot = new QWidget(this);
    m_dot->setFixedSize(7, 7);
    m_dot->setStyleSheet("background: rgb(96, 103, 138); border-radius: 3px;");
    
    m_label = new QLabel(text, this);
    m_label->setStyleSheet("font-size: 11px; color: rgb(179, 185, 214);");
    
    layout->addWidget(m_dot);
    layout->addWidget(m_label);
    
    setLayout(layout);
    updateStyle();
}

void PillWidget::setActive(bool active) {
    m_active = active;
    updateStyle();
}

void PillWidget::updateStyle() {
    if (m_active) {
        setStyleSheet("QWidget { background: rgba(255, 182, 73, 31); border: 1px solid rgb(255, 182, 73); border-radius: 999px; }");
        m_dot->setStyleSheet("background: rgb(255, 182, 73); border-radius: 3px;");
        m_label->setStyleSheet("font-size: 11px; color: rgb(255, 224, 166);");
    } else {
        setStyleSheet("QWidget { background: transparent; border: 1px solid rgba(255, 255, 255, 31); border-radius: 999px; }");
        m_dot->setStyleSheet("background: rgb(96, 103, 138); border-radius: 3px;");
        m_label->setStyleSheet("font-size: 11px; color: rgb(179, 185, 214);");
    }
}
