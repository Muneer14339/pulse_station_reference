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
    auto* gridPage = new QWidget(m_stack);
    gridPage->setStyleSheet(AppTheme::transparent());
    auto* gvb = new QVBoxLayout(gridPage);
    gvb->setContentsMargins(0, 0, 0, 0);

    auto* scroll = new QScrollArea(gridPage);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget;
    container->setStyleSheet(AppTheme::transparent());
    m_grid = new QGridLayout(container);
    m_grid->setContentsMargins(16, 16, 16, 16);
    m_grid->setSpacing(10);
    m_grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    container->setLayout(m_grid);
    scroll->setWidget(container);
    gvb->addWidget(scroll, 1);
    gridPage->setLayout(gvb);

    // ── Page 1: lightbox screen ───────────────────────────────────────────────
    m_lightbox = new AlbumLightboxScreen(m_stack);
    connect(m_lightbox, &AlbumLightboxScreen::closeRequested,
            this, [this]{ m_stack->setCurrentIndex(0); });

    m_stack->addWidget(gridPage);   // 0
    m_stack->addWidget(m_lightbox); // 1

    vb->addWidget(m_stack, 1);
    setLayout(vb);
}

QWidget* SessionAlbumTab::makeCard(const ShotRecord& s, int index) {
    auto* card = new QWidget(this);
    card->setFixedSize(CARD_SIZE, CARD_SIZE);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setStyleSheet(AppTheme::deviceCard());
    card->setCursor(Qt::PointingHandCursor);

    // Image — fills entire card
    auto* imgLbl = new QLabel(card);
    imgLbl->setGeometry(0, 0, CARD_SIZE, CARD_SIZE);
    imgLbl->setAlignment(Qt::AlignCenter);
    imgLbl->setStyleSheet(AppTheme::cameraView());

    if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath)) {
        QPixmap px(s.imagePath);
        if (!px.isNull()) {
            imgLbl->setPixmap(
                px.scaled(QSize(CARD_SIZE, CARD_SIZE),
                          Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        } else {
            imgLbl->setText(QString("Shot #%1").arg(s.number));
        }
    } else {
        imgLbl->setText(QString("Shot #%1").arg(s.number));
    }

    // Caption — overlaid at bottom, with horizontal padding so text doesn't touch edges
    auto* caption = new QWidget(card);
    caption->setGeometry(0, CARD_SIZE - CAPTION_H, CARD_SIZE, CAPTION_H);
    caption->setAttribute(Qt::WA_StyledBackground, true);
    caption->setStyleSheet(AppTheme::gridHeader());  // app-standard overlay bar style

    auto* cl = new QHBoxLayout(caption);
    // Margins: 12px left/right so text has breathing room from card border
    // 0 top/bottom since caption bar itself has fixed height
    cl->setContentsMargins(12, 0, 12, 0);
    auto* lbl = new QLabel(
        QString("Shot %1  ·  Score %2").arg(s.number).arg(s.score), caption);
    lbl->setStyleSheet(AppTheme::summaryRowLabel());
    lbl->setAlignment(Qt::AlignCenter);
    cl->addWidget(lbl);
    caption->setLayout(cl);

    // Transparent click overlay
    auto* btn = new QPushButton(card);
    btn->setGeometry(0, 0, CARD_SIZE, CARD_SIZE);
    btn->setStyleSheet(AppTheme::transparent());
    btn->raise();
    int i = index;
    connect(btn, &QPushButton::clicked, this, [this, i]{ openLightbox(i); });

    return card;
}

void SessionAlbumTab::populate(const SessionResult& r) {
    while (m_grid->count()) {
        auto* item = m_grid->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_shots     = r.shots;
    m_sessionId = r.params.sessionId;

    for (int i = 0; i < m_shots.size(); ++i)
        m_grid->addWidget(makeCard(m_shots[i], i), i / COLS, i % COLS);

    m_stack->setCurrentIndex(0);
}

void SessionAlbumTab::openLightbox(int index) {
    m_lightbox->open(m_shots, m_sessionId, index);
    m_stack->setCurrentIndex(1);
}