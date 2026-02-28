// src/ui/review/SessionReviewScreen.cpp
#include "SessionReviewScreen.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

SessionReviewScreen::SessionReviewScreen(QWidget* parent) : QWidget(parent) {
    buildUI();
}

void SessionReviewScreen::buildUI() {
    setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    // ── Tab bar ───────────────────────────────────────────────────────────
    auto* tabBar = new QWidget(this);
    tabBar->setStyleSheet(
        "QWidget { background: rgba(9,14,27,255); border-bottom: 1px solid rgba(255,255,255,20); }");
    auto* tl = new QHBoxLayout(tabBar);
    tl->setContentsMargins(0, 0, 0, 0);
    tl->setSpacing(0);

    const QStringList tabNames = {"Shot Count", "Session Summary", "Session Album", "ShoQ Review"};
    for (int i = 0; i < tabNames.size(); ++i) {
        auto* btn = new QPushButton(tabNames[i], tabBar);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(48);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx]{ switchTab(idx); });
        tl->addWidget(btn, 1);
        m_tabBtns.append(btn);
    }
    tabBar->setLayout(tl);
    vb->addWidget(tabBar);

    // ── Content stack ─────────────────────────────────────────────────────
    m_shotCountTab = new ShotCountTab(this);
    m_summaryTab   = new SessionSummaryTab(this);
    m_albumTab     = new SessionAlbumTab(this);
    m_shoqTab      = new ShoQReviewTab(this);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_shotCountTab);
    m_stack->addWidget(m_summaryTab);
    m_stack->addWidget(m_albumTab);
    m_stack->addWidget(m_shoqTab);
    vb->addWidget(m_stack, 1);

    // ── Bottom action bar ─────────────────────────────────────────────────
    auto* bottomBar = new QWidget(this);
    bottomBar->setStyleSheet(
        "QWidget { background: rgba(9,14,27,255); border-top: 1px solid rgba(255,255,255,20); }");
    auto* bl = new QHBoxLayout(bottomBar);
    bl->setContentsMargins(24, 12, 24, 12);

    auto* discardBtn = new QPushButton("Discard Session", bottomBar);
    discardBtn->setStyleSheet(AppTheme::buttonGhost());
    discardBtn->setCursor(Qt::PointingHandCursor);
    connect(discardBtn, &QPushButton::clicked, this, &SessionReviewScreen::discardRequested);

    auto* saveBtn = new QPushButton("Save Session", bottomBar);
    saveBtn->setStyleSheet(AppTheme::buttonPrimary());
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &SessionReviewScreen::saveRequested);

    bl->addStretch();
    bl->addWidget(discardBtn);
    bl->addSpacing(12);
    bl->addWidget(saveBtn);
    bottomBar->setLayout(bl);
    vb->addWidget(bottomBar);

    setLayout(vb);
    switchTab(0);   // start on Shot Count
}

void SessionReviewScreen::switchTab(int index) {
    m_stack->setCurrentIndex(index);
    for (int i = 0; i < m_tabBtns.size(); ++i) {
        const bool active = (i == index);
        m_tabBtns[i]->setStyleSheet(active
            ? R"(
                QPushButton {
                    background: rgba(255,182,73,18);
                    border-bottom: 3px solid rgb(255,182,73);
                    color: rgb(255,182,73);
                    font-weight: 600;
                    font-size: 15px;
                    border-top: none; border-left: none; border-right: none;
                })"
            : R"(
                QPushButton {
                    background: transparent;
                    border: none;
                    color: rgb(140,147,181);
                    font-size: 15px;
                }
                QPushButton:hover { background: rgba(255,255,255,8); color: rgb(210,215,245); })");
    }
}

void SessionReviewScreen::populate(const SessionResult& result) {
    m_shotCountTab->populate(result);
    m_summaryTab->populate(result);
    m_albumTab->populate(result);
    m_shoqTab->populate(result);
    switchTab(0);
}
