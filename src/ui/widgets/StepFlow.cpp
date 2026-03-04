#include "StepFlow.h"
#include "common/AppTheme.h"

StepFlow::StepFlow(QWidget* parent) : QWidget(parent) {
    using namespace AppTheme;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, ItemGap);
    layout->setSpacing(InlineGap);
    setStyleSheet(AppTheme::transparent());

    m_stepLabel = new QLabel("STEP 0 \u00B7 READY", this);
    m_stepLabel->setStyleSheet(AppTheme::stepLabel());

    auto* pillContainer = new QWidget(this);
    pillContainer->setStyleSheet(AppTheme::transparent());
    auto* pillLayout = new QHBoxLayout(pillContainer);
    pillLayout->setContentsMargins(0, 0, 0, 0);
    pillLayout->setSpacing(InlineGap);

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
        "STEP 0 \u00B7 READY",    "STEP 1 \u00B7 CATEGORY", "STEP 2 \u00B7 CALIBER",
        "STEP 3 \u00B7 PROFILE",  "STEP 4 \u00B7 DISTANCE", "STEP 5 \u00B7 DRILL"
    };
    m_stepLabel->setText((step >= 0 && step < names.size()) ? names[step] : names[0]);
    for (int i = 0; i < m_pills.size(); ++i)
        m_pills[i]->setActive(i < step);
}