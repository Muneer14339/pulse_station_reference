// src/ui/review/SessionReviewScreen.cpp
#include "SessionReviewScreen.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

SessionReviewScreen::SessionReviewScreen(QWidget* parent) : QWidget(parent) {
    buildUI();
}

void SessionReviewScreen::buildUI() {
    using namespace AppTheme;

    setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    // ── Tab bar ───────────────────────────────────────────────────────────
    auto* tabBar = new QWidget(this);
    tabBar->setAttribute(Qt::WA_StyledBackground, true);
    tabBar->setStyleSheet(AppTheme::transparent());
    auto* tl = new QHBoxLayout(tabBar);
    tl->setContentsMargins(ContentH, 0, ContentH, 0);
    tl->setSpacing(0);   // tabs sit flush — underline creates the separation

    const QStringList tabNames = {"Shot Count", "Session Summary", "Session Album", "ShoQ Review"};
    for (int i = 0; i < tabNames.size(); ++i) {
        auto* btn = new QPushButton(tabNames[i], tabBar);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(TabHeight);
        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx]{ switchTab(idx); });
        tl->addWidget(btn, 1);
        m_tabBtns.append(btn);
    }
    tabBar->setLayout(tl);
    vb->addWidget(tabBar);

    // Divider below tab bar
    auto* tabDiv = new QWidget(this);
    tabDiv->setAttribute(Qt::WA_StyledBackground, true);
    tabDiv->setFixedHeight(1);
    tabDiv->setStyleSheet(AppTheme::divider());
    vb->addWidget(tabDiv);

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

    // Divider above bottom bar
    auto* bottomDiv = new QWidget(this);
    bottomDiv->setAttribute(Qt::WA_StyledBackground, true);
    bottomDiv->setFixedHeight(1);
    bottomDiv->setStyleSheet(AppTheme::divider());
    vb->addWidget(bottomDiv);

    // ── Bottom action bar ─────────────────────────────────────────────────
    auto* bottomBar = new QWidget(this);
    bottomBar->setAttribute(Qt::WA_StyledBackground, true);
    bottomBar->setStyleSheet(AppTheme::transparent());
    auto* bl = new QHBoxLayout(bottomBar);
    bl->setContentsMargins(ContentH, ItemGap, ContentH, ItemGap);
    bl->setSpacing(InlineGap);

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
    bl->addWidget(saveBtn);
    bottomBar->setLayout(bl);
    vb->addWidget(bottomBar);

    setLayout(vb);
    switchTab(0);
}

void SessionReviewScreen::switchTab(int index) {
    m_stack->setCurrentIndex(index);
    for (int i = 0; i < m_tabBtns.size(); ++i)
        m_tabBtns[i]->setStyleSheet(
            i == index ? AppTheme::tabActive() : AppTheme::tabInactive());
}

void SessionReviewScreen::populate(const SessionResult& result) {
    m_shotCountTab->populate(result);
    m_summaryTab->populate(result);
    m_albumTab->populate(result);
    m_shoqTab->populate(result);
    switchTab(0);
}