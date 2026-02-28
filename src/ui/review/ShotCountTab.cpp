// src/ui/review/ShotCountTab.cpp
#include "ShotCountTab.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>

ShotCountTab::ShotCountTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());
    auto* hl = new QHBoxLayout(this);
    hl->setContentsMargins(20, 20, 20, 20);
    hl->setSpacing(16);

    // ── Left 55%: shared ShotGridWidget ──────────────────────────────────
    m_shotGrid = new ShotGridWidget(this);

    // ── Right 45%: 3 info panels (scrollable) ────────────────────────────
    auto* rightScroll = new QScrollArea(this);
    rightScroll->setWidgetResizable(true);
    rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    rightScroll->setStyleSheet(AppTheme::scrollArea());
    rightScroll->setFrameShape(QFrame::NoFrame);

    auto* rightWidget = new QWidget;
    rightWidget->setStyleSheet(AppTheme::transparent());
    auto* rvb = new QVBoxLayout(rightWidget);
    rvb->setContentsMargins(0, 0, 4, 0);
    rvb->setSpacing(12);

    m_paramsPanel  = new ReviewPanel("", "Session Parameters",  rightWidget);
    m_statsPanel   = new ReviewPanel("", "Session Stats",       rightWidget);
    m_metricsPanel = new ReviewPanel("", "Performance Metrics", rightWidget);

    rvb->addWidget(m_paramsPanel);
    rvb->addWidget(m_statsPanel);
    rvb->addWidget(m_metricsPanel);
    rvb->addStretch();
    rightWidget->setLayout(rvb);
    rightScroll->setWidget(rightWidget);

    hl->addWidget(m_shotGrid,  55);
    hl->addWidget(rightScroll, 45);
    setLayout(hl);
}

void ShotCountTab::populate(const SessionResult& r) {
    // ── Shot grid ─────────────────────────────────────────────────────────
    m_shotGrid->populate(r.shots);

    // ── Session Parameters panel ──────────────────────────────────────────
    m_paramsPanel->clearRows();
    m_paramsPanel->addRow("Session ID",  r.params.sessionId.isEmpty() ? "—" : r.params.sessionId);
    m_paramsPanel->addRow("Firearm",     r.params.firearm.isEmpty()   ? "—" : r.params.firearm);
    m_paramsPanel->addRow("Weapon Type", r.params.weaponType.isEmpty()? "—" : r.params.weaponType);
    m_paramsPanel->addRow("Distance",    r.params.distance > 0
                                           ? QString::number(r.params.distance) + " yds"
                                           : "—");
    m_paramsPanel->addRow("Drill",       r.params.drillId.isEmpty()   ? "—" : r.params.drillId);

    // ── Session Stats panel ───────────────────────────────────────────────
    m_statsPanel->clearRows();
    m_statsPanel->addRow("Scheduled Shots", QString::number(r.params.shotsScheduled));
    m_statsPanel->addRow("Fired Shots",     QString::number(r.shotsFired()));
    m_statsPanel->addRow("Missing Shots",   QString::number(r.shotsMissing()));
    m_statsPanel->addRow("Total Score",     QString::number(r.totalScore()), /*highlight=*/true);

    // ── Performance Metrics panel ─────────────────────────────────────────
    m_metricsPanel->clearRows();
    m_metricsPanel->addRow("Average Score",
                            QString::number(r.avgScore(), 'f', 2));
    m_metricsPanel->addRow("Average Split Time",
                            r.avgSplit() > 0
                                ? QString::number(r.avgSplit(), 'f', 2) + "\""
                                : "—");
    m_metricsPanel->addRow("Best Split Time",
                            r.bestSplit() > 0
                                ? QString::number(r.bestSplit(), 'f', 2) + "\""
                                : "—");
}
