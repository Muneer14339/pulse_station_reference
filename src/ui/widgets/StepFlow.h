#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include "common/PillWidget.h"

/**
 * @brief Progress indicator showing the current setup step and completed pills.
 */
class StepFlow : public QWidget {
    Q_OBJECT
public:
    explicit StepFlow(QWidget* parent = nullptr);
    void setCurrentStep(int step);

private:
    QLabel*               m_stepLabel;
    QVector<PillWidget*>  m_pills;
};
