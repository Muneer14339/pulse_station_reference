#include "ConsoleWidget.h"
#include "common/AppTheme.h"

ConsoleWidget::ConsoleWidget(SessionState* state, BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_state(state), m_btManager(btManager)
{
    m_categories = DataModels::getCategories();
    buildUI();

    // Wire SessionState changes → re-render
    connect(m_state, &SessionState::stateChanged, this, &ConsoleWidget::renderAll);

    // Wire grid selections → SessionState setters
    connect(m_categoryGrid, &ButtonGrid::buttonClicked,
            [this](const QString& id) { m_state->setCategoryId(id); });
    connect(m_caliberGrid,  &ButtonGrid::buttonClicked,
            [this](const QString& id) { m_state->setCaliberId(id); });
    connect(m_profileGrid,  &ButtonGrid::buttonClicked,
            [this](const QString& id) { m_state->setProfileId(id); });
    connect(m_distanceGrid, &ButtonGrid::buttonClicked,
            [this](const QString& id) { m_state->setDistance(id.toInt()); });
    connect(m_drillGrid,    &ButtonGrid::buttonClicked,
            [this](const QString& id) { m_state->setDrillId(id); });

    connect(m_resetBtn, &QPushButton::clicked,
            [this]() { m_state->reset(); });
    connect(m_nextBtn,  &QPushButton::clicked,
            this, &ConsoleWidget::nextRequested);

    renderAll();
}

// ── UI Construction ───────────────────────────────────────────────────────────

void ConsoleWidget::buildUI() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Left: Bluetooth panel ─────────────────────────────────────────────
    m_bluetoothPanel = new BluetoothPanel(m_btManager, this);
    m_bluetoothPanel->setFixedWidth(300);
    m_bluetoothPanel->setStyleSheet(AppTheme::sidePanel());

    connect(m_bluetoothPanel, &BluetoothPanel::connectionChanged,
            [this](bool connected) { m_state->setBluetoothConnected(connected); });

    // ── Right: scrollable setup steps ────────────────────────────────────
    m_rightScroll = new QScrollArea(this);
    m_rightScroll->setWidgetResizable(true);
    m_rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_rightScroll->setStyleSheet(AppTheme::scrollArea());

    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(20, 14, 20, 32);
    rightLayout->setSpacing(18);

    m_stepFlow = new StepFlow(rightPanel);
    rightLayout->addWidget(m_stepFlow);

    // Helper: build a step block (title row + badge + ButtonGrid)
    auto addStepBlock = [&](const QString& title,
                             const QString& badgeInitial,
                             QWidget*&     outBlock,
                             ButtonGrid*&  outGrid,
                             QLabel*&      outBadge)
    {
        outBlock = new QWidget(rightPanel);
        outBlock->setStyleSheet(AppTheme::transparent());
        auto* blockLayout = new QVBoxLayout(outBlock);
        blockLayout->setContentsMargins(0, 0, 0, 0);
        blockLayout->setSpacing(6);

        auto* titleRow = new QWidget(outBlock);
        titleRow->setStyleSheet(AppTheme::transparent());
        auto* titleLayout = new QHBoxLayout(titleRow);
        titleLayout->setContentsMargins(0, 4, 0, 4);

        auto* titleLabel = new QLabel(title, titleRow);
        titleLabel->setStyleSheet(AppTheme::sectionTitle());
        titleLayout->addWidget(titleLabel);

        if (!badgeInitial.isEmpty()) {
            outBadge = new QLabel(badgeInitial, titleRow);
            outBadge->setStyleSheet(AppTheme::badge());
            titleLayout->addWidget(outBadge);
        }
        titleLayout->addStretch();
        titleRow->setLayout(titleLayout);

        outGrid = new ButtonGrid(3, outBlock);
        blockLayout->addWidget(titleRow);
        blockLayout->addWidget(outGrid);
        outBlock->setLayout(blockLayout);

        rightLayout->addWidget(outBlock);
    };

    // ── Step 1: Category (custom title row with hint) ─────────────────────
    auto* categoryBlock = new QWidget(rightPanel);
    categoryBlock->setStyleSheet(AppTheme::transparent());
    auto* categoryLayout = new QVBoxLayout(categoryBlock);
    categoryLayout->setContentsMargins(0, 0, 0, 0);
    categoryLayout->setSpacing(6);

    auto* catTitleRow = new QWidget(categoryBlock);
    catTitleRow->setStyleSheet(AppTheme::transparent());
    auto* catTitleLayout = new QHBoxLayout(catTitleRow);
    catTitleLayout->setContentsMargins(0, 4, 0, 4);

    auto* catTitle = new QLabel("Step 1 · Category", categoryBlock);
    catTitle->setStyleSheet(AppTheme::sectionTitle());

    auto* catHint = new QLabel("Platform type", categoryBlock);
    catHint->setStyleSheet(AppTheme::sectionHint());
    catHint->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    catTitleLayout->addWidget(catTitle);
    catTitleLayout->addStretch();
    catTitleLayout->addWidget(catHint);
    catTitleRow->setLayout(catTitleLayout);

    m_categoryGrid = new ButtonGrid(3, categoryBlock);
    categoryLayout->addWidget(catTitleRow);
    categoryLayout->addWidget(m_categoryGrid);
    categoryBlock->setLayout(categoryLayout);
    rightLayout->addWidget(categoryBlock);

    // ── Steps 2–5 ─────────────────────────────────────────────────────────
    addStepBlock("Step 2 · Caliber",  "✓ Category", m_caliberBlock,  m_caliberGrid,  m_badgeCategory);
    addStepBlock("Step 3 · Profile",  "✓ Caliber",  m_profileBlock,  m_profileGrid,  m_badgeCaliber);
    addStepBlock("Step 4 · Distance", "✓ Profile",  m_distanceBlock, m_distanceGrid, m_badgeProfile);
    addStepBlock("Step 5 · Drill",    "✓ Distance", m_drillBlock,    m_drillGrid,    m_badgeDistance);

    // ── Action buttons ────────────────────────────────────────────────────
    m_actionBlock = new QWidget(rightPanel);
    m_actionBlock->setStyleSheet(AppTheme::transparent());
    auto* actionLayout = new QHBoxLayout(m_actionBlock);
    actionLayout->setContentsMargins(0, 18, 0, 0);

    m_resetBtn = new QPushButton("Reset All", m_actionBlock);
    m_resetBtn->setStyleSheet(AppTheme::buttonGhost());
    m_resetBtn->setCursor(Qt::PointingHandCursor);

    m_nextBtn = new QPushButton("Next: Review Session →", m_actionBlock);
    m_nextBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    m_nextBtn->setEnabled(false);

    actionLayout->addStretch();
    actionLayout->addWidget(m_resetBtn);
    actionLayout->addSpacing(10);
    actionLayout->addWidget(m_nextBtn);
    m_actionBlock->setLayout(actionLayout);

    rightLayout->addWidget(m_actionBlock);
    rightLayout->addStretch();
    rightPanel->setLayout(rightLayout);

    m_rightScroll->setWidget(rightPanel);
    mainLayout->addWidget(m_bluetoothPanel);
    mainLayout->addWidget(m_rightScroll);
    setLayout(mainLayout);
}

// ── Render Methods ────────────────────────────────────────────────────────────

void ConsoleWidget::renderAll() {
    m_stepFlow->setCurrentStep(m_state->currentStep());
    renderCategories();
    renderCalibers();
    renderProfiles();
    renderDistances();
    renderDrills();
    updateNextButton();
}

void ConsoleWidget::renderCategories() {
    m_categoryGrid->clear();
    for (const Category& cat : m_categories)
        m_categoryGrid->addButton(cat.label, "Typical usage at this lane", cat.id);
    m_categoryGrid->setSelectedId(m_state->categoryId());
}

void ConsoleWidget::renderCalibers() {
    if (m_state->categoryId().isEmpty()) { m_caliberBlock->hide(); return; }
    m_caliberBlock->show();
    const Category& cat = m_categories[m_state->categoryId()];
    m_badgeCategory->setText(QString("✓ %1").arg(cat.label));
    m_caliberGrid->clear();
    for (const Caliber& cal : cat.calibers)
        m_caliberGrid->addButton(cal.label,
                                  "Common for " + cat.label.toLower() + " training",
                                  cal.id);
    m_caliberGrid->setSelectedId(m_state->caliberId());
}

void ConsoleWidget::renderProfiles() {
    if (m_state->caliberId().isEmpty()) { m_profileBlock->hide(); return; }
    m_profileBlock->show();
    const Caliber& cal = m_categories[m_state->categoryId()].calibers[m_state->caliberId()];
    m_badgeCaliber->setText(QString("✓ %1").arg(cal.label));
    m_profileGrid->clear();
    for (const Profile& prof : cal.profiles)
        m_profileGrid->addButton(prof.label, prof.desc, prof.id);
    m_profileGrid->setSelectedId(m_state->profileId());
}

void ConsoleWidget::renderDistances() {
    if (m_state->profileId().isEmpty()) { m_distanceBlock->hide(); return; }
    m_distanceBlock->show();

    // Find selected profile label for badge
    const Caliber& cal = m_categories[m_state->categoryId()].calibers[m_state->caliberId()];
    for (const Profile& prof : cal.profiles) {
        if (prof.id == m_state->profileId()) {
            m_badgeProfile->setText(QString("✓ %1").arg(prof.label));
            break;
        }
    }

    m_distanceGrid->clear();
    for (int dist : DataModels::getDistances())
        m_distanceGrid->addButton(QString("%1 Yards").arg(dist), "Set Carrier",
                                   QString::number(dist));
    m_distanceGrid->setSelectedId(QString::number(m_state->distance()));
}

void ConsoleWidget::renderDrills() {
    if (m_state->distance() == -1) { m_drillBlock->hide(); return; }
    m_drillBlock->show();
    m_badgeDistance->setText(QString("✓ %1 yds").arg(m_state->distance()));
    m_drillGrid->clear();
    for (const Drill& drl : DataModels::getDrills())
        m_drillGrid->addButton(drl.label, drl.desc, drl.id);
    m_drillGrid->setSelectedId(m_state->drillId());
}

void ConsoleWidget::updateNextButton() {
    if (!m_state->isComplete()) { m_actionBlock->hide(); return; }
    m_actionBlock->show();
    m_nextBtn->setEnabled(m_state->canProceed());
}
