#include "ButtonGrid.h"
#include "common/AppTheme.h"

ButtonGrid::ButtonGrid(int columns, QWidget* parent)
    : QWidget(parent), m_columns(columns)
{
    m_layout = new QGridLayout(this);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(0, 6, 0, 4);

    for (int i = 0; i < m_columns; ++i)
        m_layout->setColumnStretch(i, 1);

    setLayout(m_layout);
    setStyleSheet(AppTheme::transparent());
}

void ButtonGrid::addButton(const QString& mainText, const QString& subText, const QString& id) {
    auto* btn = new CustomButton(mainText, subText, this);

    const int count = m_buttonIds.size();
    m_layout->addWidget(btn, count / m_columns, count % m_columns);
    m_buttonIds[btn] = id;

    connect(btn, &QPushButton::clicked, this, &ButtonGrid::onButtonClicked);
}

void ButtonGrid::clear() {
    for (auto* btn : m_buttonIds.keys()) {
        m_layout->removeWidget(btn);
        btn->deleteLater();
    }
    m_buttonIds.clear();
    m_selectedId.clear();
}

void ButtonGrid::setSelectedId(const QString& id) {
    m_selectedId = id;
    for (auto it = m_buttonIds.begin(); it != m_buttonIds.end(); ++it)
        it.key()->setSelected(it.value() == id);
}

void ButtonGrid::onButtonClicked() {
    auto* btn = qobject_cast<CustomButton*>(sender());
    if (!btn) return;
    const QString id = m_buttonIds.value(btn);
    setSelectedId(id);
    emit buttonClicked(id);
}
