#include "SummaryBox.h"
#include "common/AppTheme.h"

SummaryBox::SummaryBox(QWidget* parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(14, 12, 14, 12);
    m_layout->setSpacing(4);
    setLayout(m_layout);
    setStyleSheet(AppTheme::summaryBox());
}

void SummaryBox::updateSummary(const QString& category, const QString& caliber,
                               const QString& profile, int distance, const QString& drill)
{
    // Clear previous rows
    while (m_layout->count() > 0) {
        QWidget* w = m_layout->itemAt(0)->widget();
        m_layout->removeWidget(w);
        delete w;
    }

    auto addRow = [this](const QString& label, const QString& value, bool highlight = false) {
        auto* row = new QWidget(this);
        row->setStyleSheet(AppTheme::transparent());
        auto* rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 2, 0, 2);

        auto* labelW = new QLabel(label.toUpper(), row);
        labelW->setStyleSheet(AppTheme::summaryRowLabel());

        auto* valueW = new QLabel(value, row);
        valueW->setStyleSheet(highlight ? AppTheme::summaryRowHighlight()
                                        : AppTheme::summaryRowValue());

        rowLayout->addWidget(labelW);
        rowLayout->addStretch();
        rowLayout->addWidget(valueW);
        row->setLayout(rowLayout);
        m_layout->addWidget(row);
    };

    addRow("Loadout",  QString("%1 / %2").arg(category, caliber));
    addRow("Profile",  profile);
    addRow("Distance", QString("%1 yds").arg(distance));
    addRow("Drill",    drill, /*highlight=*/true);
}
