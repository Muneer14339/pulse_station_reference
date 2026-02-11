#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "core/SessionState.h"
#include "core/DataModels.h"
#include "StepFlow.h"
#include "ButtonGrid.h"

class ConsoleWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConsoleWidget(SessionState* state, QWidget* parent = nullptr);
    
signals:
    void confirmSession();
    
private:
    void setupUI();
    void renderAll();
    void renderCategories();
    void renderCalibers();
    void renderProfiles();
    void renderDistances();
    void renderTargets();
    void renderDrills();
    void showNextButton();
    
    SessionState* m_state;
    QMap<QString, Category> m_categories;
    
    QScrollArea* m_rightScroll;
    
    StepFlow* m_stepFlow;
    QWidget* m_caliberBlock;
    QWidget* m_profileBlock;
    QWidget* m_distanceBlock;
    QWidget* m_targetBlock;
    QWidget* m_drillBlock;
    QWidget* m_actionBlock;
    
    ButtonGrid* m_categoryGrid;
    ButtonGrid* m_caliberGrid;
    ButtonGrid* m_profileGrid;
    ButtonGrid* m_distanceGrid;
    ButtonGrid* m_targetGrid;
    ButtonGrid* m_drillGrid;
    
    QPushButton* m_resetBtn;
    QPushButton* m_confirmBtn;
    
    QLabel* m_badgeCategory;
    QLabel* m_badgeCaliber;
    QLabel* m_badgeProfile;
    QLabel* m_badgeDistance;
    QLabel* m_badgeTarget;
};