// src/ui/review/SessionAlbumTab.cpp
#include "SessionAlbumTab.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QFile>
#include <QPushButton>

SessionAlbumTab::SessionAlbumTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());

    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    m_stack = new QStackedWidget(this);

    // ── Page 0: grid ─────────────────────────────────────────────────────────
    m_gridPage = new QWidget(m_stack);
    m_gridPage->setStyleSheet(AppTheme::transparent());
    auto* gvb = new QVBoxLayout(m_gridPage);
    gvb->setContentsMargins(0, 0, 0, 0);
    gvb->setSpacing(0);

    auto* scroll = new QScrollArea(m_gridPage);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget;
    container->setStyleSheet(AppTheme::transparent());
    m_grid = new QGridLayout(container);
    m_grid->setContentsMargins(AppTheme::ContentH, AppTheme::ContentV,
                               AppTheme::ContentH, AppTheme::ContentV);
    m_grid->setSpacing(AppTheme::ItemGap);
    m_grid->setAlignment(Qt::AlignTop);
    for (int c = 0; c < COLS; ++c)
        m_grid->setColumnStretch(c, 1);
    container->setLayout(m_grid);
    scroll->setWidget(container);
    gvb->addWidget(scroll, 1);
    m_gridPage->setLayout(gvb);

    // ── Page 1: lightbox ─────────────────────────────────────────────────────
    m_lightbox = new AlbumLightboxScreen(m_stack);
    connect(m_lightbox, &AlbumLightboxScreen::closeRequested,
            this, [this]{ m_stack->setCurrentIndex(0); });

    m_stack->addWidget(m_gridPage);
    m_stack->addWidget(m_lightbox);

    vb->addWidget(m_stack, 1);
    setLayout(vb);
}

int SessionAlbumTab::cardSize() const {
    const int available = width()
        - 2 * AppTheme::ContentH
        - (COLS - 1) * AppTheme::ItemGap;
    return qMax(100, available / COLS);
}

QWidget* SessionAlbumTab::makeCard(const ShotRecord& s, int index, int sz) {
    using namespace AppTheme;

    auto* card = new QWidget(this);
    card->setFixedSize(sz, sz);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setStyleSheet(AppTheme::deviceCard());
    card->setCursor(Qt::PointingHandCursor);

    auto* imgLbl = new QLabel(card);
    imgLbl->setGeometry(0, 0, sz, sz);
    imgLbl->setAlignment(Qt::AlignCenter);
    imgLbl->setStyleSheet(AppTheme::cameraView());

    if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath)) {
        QPixmap px(s.imagePath);
        if (!px.isNull())
            imgLbl->setPixmap(
                px.scaled(QSize(sz, sz),
                          Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        else
            imgLbl->setText(QString("Shot #%1").arg(s.number));
    } else {
        imgLbl->setText(QString("Shot #%1").arg(s.number));
    }

    // Caption overlay — inset from card edges, anchored to bottom
    const int captH  = CardPadV * 3;
    const int captMx = CardPadV;
    const int captMb = CardPadV;

    auto* caption = new QWidget(card);
    caption->setGeometry(captMx, sz - captH - captMb, sz - 2 * captMx, captH);
    caption->setAttribute(Qt::WA_StyledBackground, true);
    caption->setStyleSheet(AppTheme::captionOverlay());

    auto* cl = new QHBoxLayout(caption);
    cl->setContentsMargins(InlineGap, 0, InlineGap, 0);
    auto* lbl = new QLabel(
        QString("Shot %1  \u00B7  Score %2").arg(s.number).arg(s.score), caption);
    lbl->setStyleSheet(AppTheme::summaryRowLabel());
    lbl->setAlignment(Qt::AlignCenter);
    cl->addWidget(lbl);
    caption->setLayout(cl);

    auto* btn = new QPushButton(card);
    btn->setGeometry(0, 0, sz, sz);
    btn->setStyleSheet(AppTheme::transparent());
    btn->raise();
    const int i = index;
    connect(btn, &QPushButton::clicked, this, [this, i]{ openLightbox(i); });

    return card;
}

void SessionAlbumTab::populate(const SessionResult& r) {
    m_shots     = r.shots;
    m_sessionId = r.params.sessionId;
    m_populated = true;
    rebuildGrid();
    m_stack->setCurrentIndex(0);
}

void SessionAlbumTab::rebuildGrid() {
    if (!m_populated) return;
    while (m_grid->count()) {
        auto* item = m_grid->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    const int sz = cardSize();
    for (int i = 0; i < m_shots.size(); ++i)
        m_grid->addWidget(makeCard(m_shots[i], i, sz),
                          i / COLS, i % COLS, Qt::AlignHCenter);
}

void SessionAlbumTab::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    rebuildGrid();
}

void SessionAlbumTab::openLightbox(int index) {
    m_lightbox->open(m_shots, m_sessionId, index);
    m_stack->setCurrentIndex(1);
}