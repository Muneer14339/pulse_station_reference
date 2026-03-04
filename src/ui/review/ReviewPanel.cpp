// ════════════════════════════════════════════════════════════════════════════
//  ReviewPanel.cpp
// ════════════════════════════════════════════════════════════════════════════
#include "ReviewPanel.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>

ReviewPanel::ReviewPanel(const QString& icon, const QString& title, QWidget* parent)
    : QWidget(parent)
{
    using namespace AppTheme;

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(AppTheme::summaryBox());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(CardPadH, CardPadV, CardPadH, CardPadV);
    outer->setSpacing(ItemGap);

    auto* titleLbl = new QLabel(icon.isEmpty() ? title : icon + "  " + title, this);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());

    auto* div = new QWidget(this);
    div->setAttribute(Qt::WA_StyledBackground, true);
    div->setFixedHeight(1);
    div->setStyleSheet(AppTheme::divider());

    auto* rowsContainer = new QWidget(this);
    rowsContainer->setStyleSheet(AppTheme::transparent());
    m_rowsLayout = new QVBoxLayout(rowsContainer);
    m_rowsLayout->setContentsMargins(0, RowPad, 0, 0);
    m_rowsLayout->setSpacing(ItemGap);
    rowsContainer->setLayout(m_rowsLayout);

    outer->addWidget(titleLbl);
    outer->addWidget(div);
    outer->addWidget(rowsContainer);
    outer->addStretch();
    setLayout(outer);
}

void ReviewPanel::addRow(const QString& key, const QString& value, bool highlight) {
    using namespace AppTheme;

    auto* row = new QWidget(this);
    row->setStyleSheet(AppTheme::transparent());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, RowPad, 0, RowPad);
    rl->setSpacing(InlineGap);

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