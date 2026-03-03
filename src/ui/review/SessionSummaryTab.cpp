// src/ui/review/SessionSummaryTab.cpp
#include "SessionSummaryTab.h"
#include "common/AppTheme.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

SessionSummaryTab::SessionSummaryTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());

    auto* hl = new QHBoxLayout(this);
    hl->setContentsMargins(16, 16, 16, 16);
    hl->setSpacing(16);
    // No Qt::AlignTop — both panels use Expanding vertical policy (set in ReviewPanel)
    // so they'll be equal height, filling the available space

    m_performancePanel = new ReviewPanel("", "Performance Summary", this);
    m_paramsPanel      = new ReviewPanel("", "Session Parameters",  this);

    hl->addWidget(m_performancePanel, 1);
    hl->addWidget(m_paramsPanel,      1);
    setLayout(hl);
}

void SessionSummaryTab::populate(const SessionResult& r) {
    m_performancePanel->clearRows();
    m_performancePanel->addRow("Scheduled Shots", QString::number(r.params.shotsScheduled));
    m_performancePanel->addRow("Shots Fired",     QString::number(r.shotsFired()));
    m_performancePanel->addRow("Shots Missing",   QString::number(r.shotsMissing()));
    m_performancePanel->addRow("Total Score",     QString::number(r.totalScore()), true);
    m_performancePanel->addRow("Average Score",   QString::number(r.avgScore(), 'f', 2));
    m_performancePanel->addRow("Avg Split Time",
                                r.avgSplit() > 0
                                    ? QString::number(r.avgSplit(), 'f', 2) + "\""
                                    : "—");
    m_performancePanel->addRow("Best Split Time",
                                r.bestSplit() > 0
                                    ? QString::number(r.bestSplit(), 'f', 2) + "\""
                                    : "—");

    m_paramsPanel->clearRows();
    m_paramsPanel->addRow("Session ID",  r.params.sessionId);
    m_paramsPanel->addRow("Shooter",     r.params.shooterId);
    m_paramsPanel->addRow("Firearm",     r.params.firearm);
    m_paramsPanel->addRow("Weapon Type", r.params.weaponType);
    m_paramsPanel->addRow("Distance",    r.params.distance > 0
                                             ? QString::number(r.params.distance) + " yds"
                                             : "—");
    m_paramsPanel->addRow("Drill",       r.params.drillId);
}