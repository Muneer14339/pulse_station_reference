// src/ui/review/ReviewPanel.cpp
#include "ReviewPanel.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>

ReviewPanel::ReviewPanel(const QString& icon, const QString& title, QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(AppTheme::summaryBox());
    // Expanding vertical: both panels in a row will be equal height
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(16, 14, 16, 14);
    outer->setSpacing(8);

    auto* titleLbl = new QLabel(icon.isEmpty() ? title : icon + "  " + title, this);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());

    auto* div = new QWidget(this);
    div->setFixedHeight(1);
    div->setStyleSheet(AppTheme::divider());

    auto* rowsContainer = new QWidget(this);
    rowsContainer->setStyleSheet(AppTheme::transparent());
    m_rowsLayout = new QVBoxLayout(rowsContainer);
    m_rowsLayout->setContentsMargins(0, 4, 0, 0);
    m_rowsLayout->setSpacing(8);
    rowsContainer->setLayout(m_rowsLayout);

    outer->addWidget(titleLbl);
    outer->addWidget(div);
    outer->addWidget(rowsContainer);
    outer->addStretch(); // pushes rows to top so both panels end at same point
    setLayout(outer);
}

void ReviewPanel::addRow(const QString& key, const QString& value, bool highlight) {
    auto* row = new QWidget(this);
    row->setStyleSheet(AppTheme::transparent());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 2, 0, 2);

    // Keys: labelSecondary (17px) — same body size as rest of app, readable at 40+ age
    auto* k = new QLabel(key, row);
    k->setStyleSheet(AppTheme::labelSecondary());

    auto* v = new QLabel(value, row);
    v->setStyleSheet(highlight ? AppTheme::summaryRowHighlight()
                               : AppTheme::summaryRowValue());
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