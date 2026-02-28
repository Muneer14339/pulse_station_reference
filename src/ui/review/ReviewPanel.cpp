// src/ui/review/ReviewPanel.cpp
#include "ReviewPanel.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>

ReviewPanel::ReviewPanel(const QString& icon, const QString& title, QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(AppTheme::summaryBox());          // existing: dark panel + border

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 14, 16, 14);
    outer->setSpacing(10);

    auto* titleLbl = new QLabel(icon.isEmpty() ? title : icon + "  " + title, this);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());  // existing

    auto* div = new QWidget(this);
    div->setFixedHeight(1);
    div->setStyleSheet(AppTheme::divider());            // existing

    auto* rowsContainer = new QWidget(this);
    rowsContainer->setStyleSheet(AppTheme::transparent());
    m_rowsLayout = new QVBoxLayout(rowsContainer);
    m_rowsLayout->setContentsMargins(0, 0, 0, 0);
    m_rowsLayout->setSpacing(5);
    rowsContainer->setLayout(m_rowsLayout);

    outer->addWidget(titleLbl);
    outer->addWidget(div);
    outer->addWidget(rowsContainer);
    setLayout(outer);
}

void ReviewPanel::addRow(const QString& key, const QString& value, bool highlight) {
    auto* row = new QWidget(this);
    row->setStyleSheet(AppTheme::transparent());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 2, 0, 2);

    auto* k = new QLabel(key, row);
    k->setStyleSheet(AppTheme::summaryRowLabel());      // existing

    auto* v = new QLabel(value, row);
    v->setStyleSheet(highlight ? AppTheme::summaryRowHighlight()
                               : AppTheme::summaryRowValue());  // existing
    v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    rl->addWidget(k);
    rl->addStretch();
    rl->addWidget(v);
    row->setLayout(rl);
    m_rowsLayout->addWidget(row);
}

void ReviewPanel::clearRows() {
    while (m_rowsLayout->count()) {
        auto* item = m_rowsLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}
