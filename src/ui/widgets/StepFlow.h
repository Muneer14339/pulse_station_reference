#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include "common/PillWidget.h"

class StepFlow : public QWidget {
    Q_OBJECT
public:
    explicit StepFlow(QWidget* parent = nullptr);
    void setCurrentStep(int step);
    
private:
    QLabel* m_stepLabel;
    QHBoxLayout* m_pillLayout;
    QVector<PillWidget*> m_pills;
};
