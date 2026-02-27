#include "TrainingPlaceholder.h"
#include "common/AppTheme.h"
#include "ui/widgets/SummaryBox.h"
#include "core/DataModels.h"
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

TrainingPlaceholder::TrainingPlaceholder(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    buildUI();

    connect(m_state, &SessionState::stateChanged,
            this, &TrainingPlaceholder::updateStartButton);
    connect(m_state, &SessionState::bluetoothConnectionChanged,
            this, [this](bool) { updateStartButton(); });
    connect(m_state, &SessionState::cameraConnectionChanged,
            this, [this](bool) { updateStartButton(); });
}

void TrainingPlaceholder::buildUI() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Left: Camera panel (25% of total width) ───────────────────────────
    m_cameraPanel = new CameraPanel(this);
    m_cameraPanel->setMinimumWidth(260);
    m_cameraPanel->setStyleSheet(AppTheme::sidePanel());

    connect(m_cameraPanel, &CameraPanel::connectionChanged,
            m_state, &SessionState::setCameraConnected);

    // ── Right: scroll content (75%) ───────────────────────────────────────
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());

    auto* content = new QWidget();
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 18, 24, 36);
    layout->setSpacing(20);

    // Title row
    auto* titleRow = new QWidget(content);
    titleRow->setStyleSheet(AppTheme::transparent());
    auto* titleRowLayout = new QHBoxLayout(titleRow);
    titleRowLayout->setContentsMargins(0, 0, 0, 0);

    auto* title = new QLabel("Session Ready", titleRow);
    title->setStyleSheet(AppTheme::pageTitle());

    auto* statusBadge = new QLabel("● LIVE", titleRow);
    statusBadge->setStyleSheet(AppTheme::badgeLive());

    titleRowLayout->addWidget(title);
    titleRowLayout->addStretch();
    titleRowLayout->addWidget(statusBadge);
    titleRow->setLayout(titleRowLayout);

    auto* subtitle = new QLabel(
        "Your lane is configured. Connect a camera to begin the session.", content);
    subtitle->setStyleSheet(AppTheme::labelSecondary());

    auto* summaryTitle = new QLabel("Session Configuration", content);
    summaryTitle->setStyleSheet(AppTheme::sectionTitle());

    m_summaryBox = new SummaryBox(content);

    // Connection status hint
    auto* statusHint = new QLabel(
        "Both Bluetooth and Camera must be connected to start.", content);
    statusHint->setStyleSheet(AppTheme::labelMuted());
    statusHint->setWordWrap(true);

    // Action row
    auto* actionRow = new QWidget(content);
    actionRow->setStyleSheet(AppTheme::transparent());
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0, 18, 0, 0);

    auto* newSessionBtn = new QPushButton("← New Session", actionRow);
    newSessionBtn->setStyleSheet(AppTheme::buttonGhost());
    newSessionBtn->setCursor(Qt::PointingHandCursor);
    connect(newSessionBtn, &QPushButton::clicked,
            this, &TrainingPlaceholder::newSessionRequested);

    m_startBtn = new QPushButton("Start Training Session", actionRow);
    m_startBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_startBtn->setCursor(Qt::PointingHandCursor);
    m_startBtn->setEnabled(false);

    connect(m_startBtn, &QPushButton::clicked,
            this, &TrainingPlaceholder::sessionStarted);
    actionLayout->addWidget(newSessionBtn);
    actionLayout->addSpacing(10);
    actionLayout->addWidget(m_startBtn);
    actionRow->setLayout(actionLayout);

    layout->addWidget(titleRow);
    layout->addWidget(subtitle);
    layout->addWidget(summaryTitle);
    layout->addWidget(m_summaryBox);
    layout->addWidget(statusHint);
    layout->addWidget(actionRow);
    layout->addStretch();
    content->setLayout(layout);

    scroll->setWidget(content);
    mainLayout->addWidget(m_cameraPanel, 25);  // 25% width
    mainLayout->addWidget(scroll,        75);  // 75% width
    setLayout(mainLayout);
}

void TrainingPlaceholder::refresh() {
    if (!m_state->isComplete()) return;

    const auto categories = DataModels::getCategories();
    const Category& cat   = categories[m_state->categoryId()];
    const Caliber&  cal   = cat.calibers[m_state->caliberId()];

    Profile prof;
    for (const Profile& p : cal.profiles)
        if (p.id == m_state->profileId()) { prof = p; break; }

    Drill drl;
    for (const Drill& d : DataModels::getDrills())
        if (d.id == m_state->drillId()) { drl = d; break; }

    m_summaryBox->updateSummary(cat.label, cal.label, prof.label,
                                m_state->distance(), drl.label);
    updateStartButton();
}

void TrainingPlaceholder::updateStartButton() {
    m_startBtn->setEnabled(m_state->canStartTraining());
}