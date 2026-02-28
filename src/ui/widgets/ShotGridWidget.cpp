#include "ShotGridWidget.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

ShotGridWidget::ShotGridWidget(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());

    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    auto* header = new QWidget(this);
    header->setAttribute(Qt::WA_StyledBackground, true);
    header->setStyleSheet(AppTheme::gridHeader());
    auto* hl = new QHBoxLayout(header);
    hl->setContentsMargins(16, 8, 16, 8);
    for (const char* col : {"Shot #", "Score", "Split Time", "Direction"}) {
        auto* l = new QLabel(col, header);
        l->setStyleSheet(AppTheme::gridHeaderCell());
        l->setAlignment(Qt::AlignCenter);
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
    row->setAttribute(Qt::WA_StyledBackground, true);
    row->setStyleSheet(AppTheme::gridDataRow());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(16, 10, 16, 10);

    const QString splitStr = (s.splitTime < 0)
        ? QStringLiteral("--")
        : QString::number(s.splitTime, 'f', 2) + "\"";

    for (const QString& txt : { QString::number(s.number),
                                 QString::number(s.score),
                                 splitStr }) {
        auto* l = new QLabel(txt, row);
        l->setStyleSheet(AppTheme::gridDataCell());
        l->setAlignment(Qt::AlignCenter);
        rl->addWidget(l, 1);
    }

    // Direction — real text from ShotRecord
    const QString dirTxt = s.direction.isEmpty() ? QStringLiteral("—") : s.direction;
    auto* dirLbl = new QLabel(dirTxt, row);
    dirLbl->setStyleSheet(AppTheme::gridDataCell());
    dirLbl->setAlignment(Qt::AlignCenter);
    rl->addWidget(dirLbl, 1);

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