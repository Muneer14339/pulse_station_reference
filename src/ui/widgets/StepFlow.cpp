#include "StepFlow.h"

StepFlow::StepFlow(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 6);
    layout->setSpacing(4);
    setStyleSheet("background: transparent; border: none;");
  
    m_stepLabel = new QLabel("STEP 0 · READY", this);
    m_stepLabel->setStyleSheet("font-size: 12px; color: rgb(255, 182, 73); letter-spacing: 2px; font-weight: 600; background: transparent; border: none;");
  
    auto* pillContainer = new QWidget(this);
    pillContainer->setStyleSheet("background: transparent; border: none;");
    m_pillLayout = new QHBoxLayout(pillContainer);
    m_pillLayout->setContentsMargins(0, 0, 0, 0);
    m_pillLayout->setSpacing(8);
    
    QStringList steps = {"Category", "Caliber", "Profile", "Distance", "Target", "Drill"};
    for (const QString& step : steps) {
        auto* pill = new PillWidget(step, this);
        m_pills.append(pill);
        m_pillLayout->addWidget(pill);
    }
    m_pillLayout->addStretch();
    
    pillContainer->setLayout(m_pillLayout);
    
    layout->addWidget(m_stepLabel);
    layout->addWidget(pillContainer);
    setLayout(layout);
}

void StepFlow::setCurrentStep(int step) {
    QStringList stepNames = {"STEP 0 · READY", "STEP 1 · CATEGORY", "STEP 2 · CALIBER", 
                             "STEP 3 · PROFILE", "STEP 4 · DISTANCE", "STEP 5 · TARGET", 
                             "STEP 6 · DRILL"};
    
    QString stepName = (step >= 0 && step < stepNames.size()) ? stepNames[step] : "STEP 0 · READY";
    m_stepLabel->setText(stepName);
    
    for (int i = 0; i < m_pills.size(); ++i) {
        m_pills[i]->setActive(i < step);
    }
}
