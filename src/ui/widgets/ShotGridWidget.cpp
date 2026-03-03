// src/ui/widgets/ShotGridWidget.cpp
#include "ShotGridWidget.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

// Cell style helpers — border-left gives continuous column divider automatically
// First column has no left border; rest do. This avoids per-row QWidget dividers.
static QString cellStyle(bool firstCol, bool isHeader) {
    const QString base = isHeader ? AppTheme::summaryRowLabel()
                                  : AppTheme::summaryRowValue();
    const QString border = firstCol
        ? "border: none;"
        : "border-left: 1px solid rgba(255,255,255,20); border-top: none; "
          "border-right: none; border-bottom: none;";
    return base + " " + border;
}

// ─────────────────────────────────────────────────────────────────────────────
ShotGridWidget::ShotGridWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    // tablePanel() = summaryBox colors, border-radius:0 for table look
    setStyleSheet(AppTheme::tablePanel());

    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    // Header: bottom separator only, no surrounding rectangle
    auto* header = new QWidget(this);
    header->setAttribute(Qt::WA_StyledBackground, true);
    header->setStyleSheet(AppTheme::gridHeader());
    auto* hl = new QHBoxLayout(header);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    const char* cols[] = {"SHOT #", "SCORE", "SPLIT TIME", "DIRECTION"};
    for (int i = 0; i < 4; ++i) {
        auto* l = new QLabel(cols[i], header);
        l->setStyleSheet(cellStyle(i == 0, true));
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0, 10, 0, 10);
        hl->addWidget(l, 1);
    }
    header->setLayout(hl);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    auto* body = new QWidget;
    body->setStyleSheet(AppTheme::transparent());
    m_bodyLayout = new QVBoxLayout(body);
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    m_bodyLayout->setSpacing(0);
    m_bodyLayout->addStretch();
    body->setLayout(m_bodyLayout);
    scroll->setWidget(body);

    vb->addWidget(header);
    vb->addWidget(scroll, 1);
    setLayout(vb);
}

QWidget* ShotGridWidget::buildRow(const ShotRecord& s) {
    auto* row = new QWidget;
    row->setStyleSheet(AppTheme::transparent());

    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 0, 0, 0);
    rl->setSpacing(0);

    const QString splitStr = (s.splitTime < 0)
        ? QStringLiteral("--")
        : QString::number(s.splitTime, 'f', 2) + "\"";

    const QStringList cells = { QString::number(s.number),
                                 QString::number(s.score),
                                 splitStr,
                                 s.direction.isEmpty() ? "—" : s.direction };

    for (int i = 0; i < cells.size(); ++i) {
        auto* l = new QLabel(cells[i], row);
        l->setStyleSheet(cellStyle(i == 0, false));
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0, 12, 0, 12);
        rl->addWidget(l, 1);
    }
    row->setLayout(rl);
    return row;
}

void ShotGridWidget::addShot(const ShotRecord& s) {
    m_bodyLayout->insertWidget(m_bodyLayout->count() - 1, buildRow(s));
}

void ShotGridWidget::populate(const QVector<ShotRecord>& shots) {
    clear();
    for (const ShotRecord& s : shots)
        m_bodyLayout->insertWidget(m_bodyLayout->count() - 1, buildRow(s));
}

void ShotGridWidget::clear() {
    while (m_bodyLayout->count() > 1) {
        auto* item = m_bodyLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}