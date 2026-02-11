#include "ConsoleWidget.h"
#include "common/AppTheme.h"
#include "common/AppColors.h"

ConsoleWidget::ConsoleWidget(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state) {
    m_categories = DataModels::getCategories();
    setupUI();
    
    connect(m_state, &SessionState::stateChanged, this, &ConsoleWidget::renderAll);
    connect(m_categoryGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setCategoryId(id);
    });
    connect(m_caliberGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setCaliberId(id);
    });
    connect(m_profileGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setProfileId(id);
    });
    connect(m_distanceGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setDistance(id.toInt());
    });
    connect(m_targetGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setTargetId(id);
    });
    connect(m_drillGrid, &ButtonGrid::buttonClicked, [this](const QString& id) {
        m_state->setDrillId(id);
    });
    connect(m_resetBtn, &QPushButton::clicked, [this]() {
        m_state->reset();
    });
    connect(m_confirmBtn, &QPushButton::clicked, this, &ConsoleWidget::confirmSession);
    
    renderAll();
}

void ConsoleWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_rightScroll = new QScrollArea(this);
    m_rightScroll->setWidgetResizable(true);
    m_rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_rightScroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(18, 12, 18, 30);
    rightLayout->setSpacing(16);
    
    m_stepFlow = new StepFlow(rightPanel);
    rightLayout->addWidget(m_stepFlow);
    
    auto addSection = [&](const QString& title, const QString& badgeText, QWidget*& block, ButtonGrid*& grid, QLabel*& badge) {
        block = new QWidget(rightPanel);
        block->setStyleSheet("background: transparent; border: none;");
        auto* blockLayout = new QVBoxLayout(block);
        blockLayout->setContentsMargins(0, 0, 0, 0);
        blockLayout->setSpacing(4);
        
        auto* titleRow = new QWidget(block);
        titleRow->setStyleSheet("background: transparent; border: none;");
        auto* titleLayout = new QHBoxLayout(titleRow);
        titleLayout->setContentsMargins(0, 4, 0, 4);
        
        auto* titleLabel = new QLabel(title, titleRow);
        titleLabel->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230, 233, 255);");
        titleLayout->addWidget(titleLabel);
        
        if (!badgeText.isEmpty()) {
            badge = new QLabel(badgeText, titleRow);
            badge->setStyleSheet(
                "font-size: 11px; color: rgb(192, 197, 227); "
                "padding: 3px 8px; border-radius: 999px; "
                "border: 1px solid rgba(255, 255, 255, 36); "
                "background: transparent;"
            );
            titleLayout->addWidget(badge);
        }
        
        titleLayout->addStretch();
        titleRow->setLayout(titleLayout);
        
        grid = new ButtonGrid(3, block);
        
        blockLayout->addWidget(titleRow);
        blockLayout->addWidget(grid);
        block->setLayout(blockLayout);
        
        rightLayout->addWidget(block);
    };
    
    auto* categoryBlock = new QWidget(rightPanel);
    categoryBlock->setStyleSheet("background: transparent; border: none;");
    auto* categoryLayout = new QVBoxLayout(categoryBlock);
    categoryLayout->setContentsMargins(0, 0, 0, 0);
    
    auto* categoryTitle = new QLabel("Step 1 · Category", categoryBlock);
    categoryTitle->setStyleSheet("font-size: 14px; font-weight: 600; color: rgb(230, 233, 255);");
    
    auto* categoryHint = new QLabel("Platform type", categoryBlock);
    categoryHint->setStyleSheet("font-size: 11px; color: rgb(140, 147, 181);");
    categoryHint->setAlignment(Qt::AlignRight);
    
    auto* categoryTitleRow = new QWidget(categoryBlock);
    categoryTitleRow->setStyleSheet("background: transparent; border: none;");
    auto* categoryTitleLayout = new QHBoxLayout(categoryTitleRow);
    categoryTitleLayout->setContentsMargins(0, 4, 0, 4);
    categoryTitleLayout->addWidget(categoryTitle);
    categoryTitleLayout->addStretch();
    categoryTitleLayout->addWidget(categoryHint);
    categoryTitleRow->setLayout(categoryTitleLayout);
    
    m_categoryGrid = new ButtonGrid(3, categoryBlock);
    
    categoryLayout->addWidget(categoryTitleRow);
    categoryLayout->addWidget(m_categoryGrid);
    categoryBlock->setLayout(categoryLayout);
    rightLayout->addWidget(categoryBlock);
    
    addSection("Step 2 · Caliber", "✓ <span>Category</span>", m_caliberBlock, m_caliberGrid, m_badgeCategory);
    addSection("Step 3 · Profile", "✓ <span>Caliber</span>", m_profileBlock, m_profileGrid, m_badgeCaliber);
    addSection("Step 4 · Distance", "✓ <span>Profile</span>", m_distanceBlock, m_distanceGrid, m_badgeProfile);
    addSection("Step 5 · Target", "✓ <span>Distance</span>", m_targetBlock, m_targetGrid, m_badgeDistance);
    addSection("Step 6 · Drill", "✓ <span>Target</span>", m_drillBlock, m_drillGrid, m_badgeTarget);
    
    m_actionBlock = new QWidget(rightPanel);
    m_actionBlock->setStyleSheet("background: transparent; border: none;");
    auto* actionLayout = new QHBoxLayout(m_actionBlock);
    actionLayout->setContentsMargins(0, 16, 0, 0);
    
    m_resetBtn = new QPushButton("Reset All", m_actionBlock);
    m_resetBtn->setStyleSheet(
        "QPushButton { background: transparent; border: 1px dashed rgba(255,255,255,64); "
        "border-radius: 12px; color: rgb(179, 185, 214); padding: 12px; min-height: 40px; }"
        "QPushButton:hover { background: rgba(255,255,255,13); }"
    );
    m_resetBtn->setCursor(Qt::PointingHandCursor);
    
    m_confirmBtn = new QPushButton("Next: Review Session →", m_actionBlock);
    m_confirmBtn->setStyleSheet(AppTheme::getButtonPrimaryStyle());
    m_confirmBtn->setCursor(Qt::PointingHandCursor);
    
    actionLayout->addStretch();
    actionLayout->addWidget(m_resetBtn);
    actionLayout->addWidget(m_confirmBtn);
    m_actionBlock->setLayout(actionLayout);
    
    rightLayout->addWidget(m_actionBlock);
    rightLayout->addStretch();
    rightPanel->setLayout(rightLayout);
    
    m_rightScroll->setWidget(rightPanel);
    mainLayout->addWidget(m_rightScroll);
    setLayout(mainLayout);
}

void ConsoleWidget::renderAll() {
    m_stepFlow->setCurrentStep(m_state->currentStep());
    renderCategories();
    renderCalibers();
    renderProfiles();
    renderDistances();
    renderTargets();
    renderDrills();
    showNextButton();
}

void ConsoleWidget::renderCategories() {
    m_categoryGrid->clear();
    for (const Category& cat : m_categories) {
        m_categoryGrid->addButton(cat.label, "Typical usage at this lane", cat.id);
    }
    m_categoryGrid->setSelectedId(m_state->categoryId());
}

void ConsoleWidget::renderCalibers() {
    if (m_state->categoryId().isEmpty()) {
        m_caliberBlock->hide();
        return;
    }
    m_caliberBlock->show();
    
    const Category& cat = m_categories[m_state->categoryId()];
    m_badgeCategory->setText(QString("✓ <span>%1</span>").arg(cat.label));
    
    m_caliberGrid->clear();
    for (const Caliber& cal : cat.calibers) {
        m_caliberGrid->addButton(cal.label, "Common for " + cat.label.toLower() + " training", cal.id);
    }
    m_caliberGrid->setSelectedId(m_state->caliberId());
}

void ConsoleWidget::renderProfiles() {
    if (m_state->caliberId().isEmpty()) {
        m_profileBlock->hide();
        return;
    }
    m_profileBlock->show();
    
    const Category& cat = m_categories[m_state->categoryId()];
    const Caliber& cal = cat.calibers[m_state->caliberId()];
    m_badgeCaliber->setText(QString("✓ <span>%1</span>").arg(cal.label));
    
    m_profileGrid->clear();
    for (const Profile& prof : cal.profiles) {
        m_profileGrid->addButton(prof.label, prof.desc, prof.id);
    }
    m_profileGrid->setSelectedId(m_state->profileId());
}

void ConsoleWidget::renderDistances() {
    if (m_state->profileId().isEmpty()) {
        m_distanceBlock->hide();
        return;
    }
    m_distanceBlock->show();
    
    const Category& cat = m_categories[m_state->categoryId()];
    const Caliber& cal = cat.calibers[m_state->caliberId()];
    
    Profile selectedProfile;
    for (const Profile& prof : cal.profiles) {
        if (prof.id == m_state->profileId()) {
            selectedProfile = prof;
            break;
        }
    }
    m_badgeProfile->setText(QString("✓ <span>%1</span>").arg(selectedProfile.label));
    
    m_distanceGrid->clear();
    for (int dist : DataModels::getDistances()) {
        m_distanceGrid->addButton(QString("%1 Yards").arg(dist), "Set Carrier", QString::number(dist));
    }
    m_distanceGrid->setSelectedId(QString::number(m_state->distance()));
}

void ConsoleWidget::renderTargets() {
    if (m_state->distance() == -1) {
        m_targetBlock->hide();
        return;
    }
    m_targetBlock->show();
    
    m_badgeDistance->setText(QString("✓ <span>%1 yds</span>").arg(m_state->distance()));
    
    m_targetGrid->clear();
    for (const Target& tgt : DataModels::getTargets()) {
        m_targetGrid->addButton(tgt.label, tgt.desc, tgt.id);
    }
    m_targetGrid->setSelectedId(m_state->targetId());
}

void ConsoleWidget::renderDrills() {
    if (m_state->targetId().isEmpty()) {
        m_drillBlock->hide();
        return;
    }
    m_drillBlock->show();
    
    Target selectedTarget;
    for (const Target& tgt : DataModels::getTargets()) {
        if (tgt.id == m_state->targetId()) {
            selectedTarget = tgt;
            break;
        }
    }
    m_badgeTarget->setText(QString("✓ <span>%1</span>").arg(selectedTarget.label));
    
    m_drillGrid->clear();
    for (const Drill& drl : DataModels::getDrills()) {
        m_drillGrid->addButton(drl.label, drl.desc, drl.id);
    }
    m_drillGrid->setSelectedId(m_state->drillId());
}

void ConsoleWidget::showNextButton() {
    if (!m_state->isComplete()) {
        m_actionBlock->hide();
        return;
    }
    m_actionBlock->show();
    m_confirmBtn->setEnabled(true);
}