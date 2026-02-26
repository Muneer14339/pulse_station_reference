#include "SummaryBox.h"

SummaryBox::SummaryBox(QWidget* parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 8, 10, 8);
    m_layout->setSpacing(3);
    setLayout(m_layout);
    
    setStyleSheet(
        "background: rgba(12, 18, 32, 242); "
        "border: 1px solid rgba(255, 255, 255, 26); "
        "border-radius: 10px;"
    );
}

void SummaryBox::updateSummary(const QString& category, const QString& caliber,
                               const QString& profile, int distance, const QString& drill) {
    while (m_layout->count() > 0) {
        QWidget* widget = m_layout->itemAt(0)->widget();
        m_layout->removeWidget(widget);
        delete widget;
    }
    
    auto addRow = [this](const QString& label, const QString& value, bool highlight = false) {
    auto* row = new QWidget(this);
    row->setStyleSheet("background: transparent; border: none;");
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    
    auto* labelWidget = new QLabel(label.toUpper(), row);
    labelWidget->setStyleSheet("font-size: 11px; color: rgb(157, 164, 197); letter-spacing: 1px; background: transparent; border: none;");
    
    auto* valueWidget = new QLabel(value, row);
    QString color = highlight ? "rgb(255, 182, 73)" : "rgb(212, 216, 243)";
    valueWidget->setStyleSheet(QString("font-size: 13px; color: %1; background: transparent; border: none;").arg(color));
    
    rowLayout->addWidget(labelWidget);
    rowLayout->addStretch();
    rowLayout->addWidget(valueWidget);
    row->setLayout(rowLayout);
    
    m_layout->addWidget(row);
};
    
    addRow("Loadout", QString("%1 / %2").arg(category, caliber));
    addRow("Profile", profile);
    addRow("Distance", QString("%1 yds").arg(distance));
    addRow("Drill", drill, true);
}
