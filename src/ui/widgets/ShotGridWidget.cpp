// src/ui/widgets/ShotGridWidget.cpp
#include "ShotGridWidget.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QResizeEvent>
#include "common/theme/Icons.h" 

// Column count — used to calculate divider positions
static constexpr int COL_COUNT = 4;
// static constexpr auto kHourglass = "\u23F3";  // ⏳

ShotGridWidget::ShotGridWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(AppTheme::tablePanel());

    // ── Header ───────────────────────────────────────────────────────────────
    m_header = new QWidget(this);
    m_header->setAttribute(Qt::WA_StyledBackground, true);
    m_header->setStyleSheet(AppTheme::gridHeader());
    auto* hl = new QHBoxLayout(m_header);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);
    for (const char* col : {"SHOT #", "SCORE", "SPLIT TIME", "DIRECTION"}) {
        auto* l = new QLabel(col, m_header);
        l->setStyleSheet(AppTheme::summaryRowLabel());
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0, 10, 0, 10);
        hl->addWidget(l, 1);
    }
    m_header->setLayout(hl);

    // ── Scroll body ──────────────────────────────────────────────────────────
    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroll->setStyleSheet(AppTheme::scrollArea());
    m_scroll->setFrameShape(QFrame::NoFrame);

    auto* body = new QWidget;
    body->setStyleSheet(AppTheme::transparent());
    m_bodyLayout = new QVBoxLayout(body);
    m_bodyLayout->setContentsMargins(0, 0, 0, 0);
    m_bodyLayout->setSpacing(0);
    m_bodyLayout->addStretch();
    body->setLayout(m_bodyLayout);
    m_scroll->setWidget(body);

    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);
    vb->addWidget(m_header);
    vb->addWidget(m_scroll, 1);
    setLayout(vb);

    // ── Overlay column dividers (3 lines for 4 columns) ──────────────────────
    // These are placed as children of `this`, raised above header + scroll,
    // and repositioned in resizeEvent to always span full widget height.
    for (int i = 0; i < COL_COUNT - 1; ++i) {
        auto* div = new QWidget(this);
        div->setAttribute(Qt::WA_StyledBackground, true);
        div->setStyleSheet(AppTheme::columnDivider());
        div->setFixedWidth(1);
        div->raise();
        m_colDividers.append(div);
    }
}

void ShotGridWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    repositionDividers();
}

void ShotGridWidget::repositionDividers() {
    const int w = width();
    const int h = height();
    const int colW = w / COL_COUNT;
    for (int i = 0; i < m_colDividers.size(); ++i) {
        const int x = colW * (i + 1);
        m_colDividers[i]->setGeometry(x, 0, 1, h);  // full height of widget
    }
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

    const QStringList texts = {
        QString::number(s.number),
        QString::number(s.score),
        splitStr,
        s.direction.isEmpty() ? QStringLiteral("\u2014") : s.direction
    };

    for (int i = 0; i < 4; ++i) {
        auto* l = new QLabel(row);
        if (i == 3 && s.missed) {
            l->setText(QStringLiteral("\u25CF"));        // ● red dot
            l->setStyleSheet(AppTheme::gridMissedCell());
        } else {
            l->setText(texts[i]);
            l->setStyleSheet(AppTheme::summaryRowValue());
        }
        l->setAlignment(Qt::AlignCenter);
        l->setContentsMargins(0, 12, 0, 12);
        rl->addWidget(l, 1);
    }
    row->setLayout(rl);
    return row;
}

QWidget* ShotGridWidget::buildPendingRow(int shotNumber) {
    auto* row = new QWidget;
    row->setStyleSheet(AppTheme::transparent());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 0, 0, 0);
    rl->setSpacing(0);
 
    for (int i = 0; i < 4; ++i) {
        if (i == 0) {
            auto* l = new QLabel(QString::number(shotNumber), row);
            l->setStyleSheet(AppTheme::summaryRowValue());
            l->setAlignment(Qt::AlignCenter);
            l->setContentsMargins(0, 12, 0, 12);
            rl->addWidget(l, 1);
        } else {
            // Wrapper — column stretch sahi rakhe, hourglass centre mein rahe
            auto* cell = new QWidget(row);
            cell->setStyleSheet(AppTheme::transparent());
            auto* cl = new QHBoxLayout(cell);
            cl->setContentsMargins(0, 8, 0, 8);
            cl->setSpacing(0);
            cl->addStretch();
            cl->addWidget(AppIcons::pendingLabel(cell));
            cl->addStretch();
            cell->setLayout(cl);
            rl->addWidget(cell, 1);   // stretch 1 = equal column width
        }
    }
    row->setLayout(rl);
    return row;
}

void ShotGridWidget::setPendingShot(int shotNumber) {
    if (m_pendingRow) {
        m_bodyLayout->removeWidget(m_pendingRow);
        m_pendingRow->deleteLater();
    }
    m_pendingRow = buildPendingRow(shotNumber);
    m_bodyLayout->insertWidget(m_bodyLayout->count() - 1, m_pendingRow);
}

void ShotGridWidget::finalizePendingRow(const ShotRecord& s) {
    if (!m_pendingRow) { addShot(s); return; }   // camera-only — no pending row exists
    const int idx = m_bodyLayout->indexOf(m_pendingRow);
    m_bodyLayout->removeWidget(m_pendingRow);
    m_pendingRow->deleteLater();
    m_pendingRow = nullptr;
    m_bodyLayout->insertWidget(idx >= 0 ? idx : m_bodyLayout->count() - 1, buildRow(s));
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
    m_pendingRow = nullptr;   // will be deleted by the loop below
    while (m_bodyLayout->count() > 1) {
        auto* item = m_bodyLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}