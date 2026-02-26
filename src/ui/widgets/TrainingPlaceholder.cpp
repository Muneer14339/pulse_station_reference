#include "TrainingPlaceholder.h"
#include "common/AppTheme.h"
#include "core/DataModels.h"
#include <QScrollArea>

TrainingPlaceholder::TrainingPlaceholder(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    buildUI();
}

void TrainingPlaceholder::refresh() {
    if (!m_state->isComplete()) return;

    // Resolve labels from IDs
    const auto categories = DataModels::getCategories();
    const Category& cat = categories[m_state->categoryId()];
    const Caliber&  cal = cat.calibers[m_state->caliberId()];

    Profile selectedProfile;
    for (const Profile& prof : cal.profiles)
        if (prof.id == m_state->profileId()) { selectedProfile = prof; break; }

    Drill selectedDrill;
    for (const Drill& drl : DataModels::getDrills())
        if (drl.id == m_state->drillId()) { selectedDrill = drl; break; }

    m_summaryBox->updateSummary(
        cat.label, cal.label, selectedProfile.label,
        m_state->distance(), selectedDrill.label);
}

void TrainingPlaceholder::buildUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());

    auto* content = new QWidget();
    auto* layout  = new QVBoxLayout(content);
    layout->setContentsMargins(24, 18, 24, 36);
    layout->setSpacing(20);

    // ── Page title ────────────────────────────────────────────────────────
    auto* titleRow = new QWidget(content);
    titleRow->setStyleSheet(AppTheme::transparent());
    auto* titleRowLayout = new QHBoxLayout(titleRow);
    titleRowLayout->setContentsMargins(0, 0, 0, 0);

    auto* title = new QLabel("Session Ready", titleRow);
    title->setStyleSheet(AppTheme::pageTitle());

    // Status badge
    auto* statusBadge = new QLabel("● LIVE", titleRow);
    statusBadge->setStyleSheet(AppTheme::badgeLive());

    titleRowLayout->addWidget(title);
    titleRowLayout->addStretch();
    titleRowLayout->addWidget(statusBadge);
    titleRow->setLayout(titleRowLayout);

    auto* subtitle = new QLabel(
        "Your lane is configured. The training session is ready to begin.", content);
    subtitle->setStyleSheet(AppTheme::labelSecondary());

    // ── Session summary ───────────────────────────────────────────────────
    auto* summaryTitle = new QLabel("Session Configuration", content);
    summaryTitle->setStyleSheet(AppTheme::sectionTitle());

    m_summaryBox = new SummaryBox(content);

    // ── Camera placeholder ────────────────────────────────────────────────
    auto* cameraTitle = new QLabel("Camera Feed", content);
    cameraTitle->setStyleSheet(AppTheme::sectionTitle());

    m_cameraPlaceholder = new QWidget(content);
    m_cameraPlaceholder->setMinimumHeight(320);
    m_cameraPlaceholder->setStyleSheet(AppTheme::trainingPlaceholderBox());

    auto* camLayout = new QVBoxLayout(m_cameraPlaceholder);
    camLayout->setAlignment(Qt::AlignCenter);
    camLayout->setSpacing(12);

    auto* camIcon = new QLabel("⬡", m_cameraPlaceholder);
    camIcon->setStyleSheet(AppTheme::trainingPlaceholderIcon());
    camIcon->setAlignment(Qt::AlignCenter);

    auto* camTitle = new QLabel("Training Feature — Coming Soon", m_cameraPlaceholder);
    camTitle->setStyleSheet(AppTheme::trainingPlaceholderTitle());
    camTitle->setAlignment(Qt::AlignCenter);

    auto* camBody = new QLabel(
        "Camera streaming and shot-processing will appear here.\n"
        "Replace this widget with the CameraWidget once available.",
        m_cameraPlaceholder);
    camBody->setStyleSheet(AppTheme::trainingPlaceholderBody());
    camBody->setAlignment(Qt::AlignCenter);
    camBody->setWordWrap(true);

    camLayout->addWidget(camIcon);
    camLayout->addWidget(camTitle);
    camLayout->addWidget(camBody);
    m_cameraPlaceholder->setLayout(camLayout);

    // ── Action ────────────────────────────────────────────────────────────
    auto* actionRow = new QWidget(content);
    actionRow->setStyleSheet(AppTheme::transparent());
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0, 10, 0, 0);

    m_newSessionBtn = new QPushButton("Start New Session", actionRow);
    m_newSessionBtn->setStyleSheet(AppTheme::buttonGhost());
    m_newSessionBtn->setCursor(Qt::PointingHandCursor);
    m_newSessionBtn->setFixedWidth(320);

    actionLayout->addStretch();
    actionLayout->addWidget(m_newSessionBtn);
    actionRow->setLayout(actionLayout);

    // ── Assemble ──────────────────────────────────────────────────────────
    layout->addWidget(titleRow);
    layout->addWidget(subtitle);
    layout->addWidget(summaryTitle);
    layout->addWidget(m_summaryBox);
    layout->addWidget(cameraTitle);
    layout->addWidget(m_cameraPlaceholder);
    layout->addWidget(actionRow);
    layout->addStretch();
    content->setLayout(layout);

    scroll->setWidget(content);
    mainLayout->addWidget(scroll);
    setLayout(mainLayout);

    connect(m_newSessionBtn, &QPushButton::clicked,
            this, &TrainingPlaceholder::newSessionRequested);
}
