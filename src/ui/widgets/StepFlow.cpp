#include "StepFlow.h"
#include "common/AppTheme.h"

StepFlow::StepFlow(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 8);
    layout->setSpacing(6);
    setStyleSheet(AppTheme::transparent());

    m_stepLabel = new QLabel("STEP 0 · READY", this);
    m_stepLabel->setStyleSheet(AppTheme::stepLabel());

    auto* pillContainer = new QWidget(this);
    pillContainer->setStyleSheet(AppTheme::transparent());
    auto* pillLayout = new QHBoxLayout(pillContainer);
    pillLayout->setContentsMargins(0, 0, 0, 0);
    pillLayout->setSpacing(10);

    const QStringList steps = {"Category", "Caliber", "Profile", "Distance", "Drill"};
    for (const QString& step : steps) {
        auto* pill = new PillWidget(step, this);
        m_pills.append(pill);
        pillLayout->addWidget(pill);
    }
    pillLayout->addStretch();
    pillContainer->setLayout(pillLayout);

    layout->addWidget(m_stepLabel);
    layout->addWidget(pillContainer);
    setLayout(layout);
}

void StepFlow::setCurrentStep(int step) {
    static const QStringList names = {
        "STEP 0 · READY",    "STEP 1 · CATEGORY", "STEP 2 · CALIBER",
        "STEP 3 · PROFILE",  "STEP 4 · DISTANCE", "STEP 5 · DRILL"
    };
    m_stepLabel->setText((step >= 0 && step < names.size()) ? names[step] : names[0]);

    for (int i = 0; i < m_pills.size(); ++i)
        m_pills[i]->setActive(i < step);
}
