// ════════════════════════════════════════════════════════════════════════════
//  ReviewScreen.cpp
// ════════════════════════════════════════════════════════════════════════════
#include "ReviewScreen.h"
#include "common/AppTheme.h"
#include <QScrollArea>

ReviewScreen::ReviewScreen(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    using namespace AppTheme;
    m_categories = DataModels::getCategories();

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());

    auto* content = new QWidget();
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(ContentH, ContentV, ContentH, ContentV * 2);
    layout->setSpacing(SectionGap);

    auto* title = new QLabel("Review Your Session", content);
    title->setStyleSheet(AppTheme::pageTitle());

    auto* subtitle = new QLabel(
        "Please review your configuration before starting the session.", content);
    subtitle->setStyleSheet(AppTheme::labelSecondary());

    m_summaryBox = new SummaryBox(content);

    auto* actionRow    = new QWidget(content);
    actionRow->setStyleSheet(AppTheme::transparent());
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0, SpaceS, 0, 0);
    actionLayout->setSpacing(InlineGap);

    m_backBtn = new QPushButton("\u2190 Back to Edit", actionRow);
    m_backBtn->setStyleSheet(AppTheme::buttonGhost());
    m_backBtn->setCursor(Qt::PointingHandCursor);

    m_confirmBtn = new QPushButton("Confirm & Begin Session", actionRow);
    m_confirmBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_confirmBtn->setCursor(Qt::PointingHandCursor);

    actionLayout->addStretch();
    actionLayout->addWidget(m_backBtn);
    actionLayout->addWidget(m_confirmBtn);
    actionRow->setLayout(actionLayout);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(m_summaryBox);
    layout->addWidget(actionRow);
    layout->addStretch();
    content->setLayout(layout);

    scroll->setWidget(content);
    mainLayout->addWidget(scroll);
    setLayout(mainLayout);

    connect(m_backBtn,    &QPushButton::clicked, this, &ReviewScreen::backRequested);
    connect(m_confirmBtn, &QPushButton::clicked, this, &ReviewScreen::confirmRequested);
}

void ReviewScreen::updateReview() {
    if (!m_state->isComplete()) return;
    const Category& cat = m_categories[m_state->categoryId()];
    const Caliber&  cal = cat.calibers[m_state->caliberId()];
    Profile selectedProfile;
    for (const Profile& prof : cal.profiles)
        if (prof.id == m_state->profileId()) { selectedProfile = prof; break; }
    Drill selectedDrill;
    for (const Drill& drl : DataModels::getDrills())
        if (drl.id == m_state->drillId()) { selectedDrill = drl; break; }
    m_summaryBox->updateSummary(cat.label, cal.label, selectedProfile.label,
                                m_state->distance(), selectedDrill.label);
}