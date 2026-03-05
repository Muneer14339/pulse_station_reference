#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include "core/SessionState.h"
#include "core/DataModels.h"
#include "core/BluetoothManager.h"
#include "StepFlow.h"
#include "ButtonGrid.h"
#include "BluetoothPanel.h"

class ConsoleWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConsoleWidget(SessionState*     state,
                           BluetoothManager* btManager,
                           QWidget*          parent = nullptr);

signals:
    void nextRequested();

private:
    void buildUI();
    void renderAll();
    void renderCategories();
    void renderCalibers();
    void renderProfiles();
    void renderDistances();
    void renderDrills();
    void updateNextButton();
    void scrollToWidget(QWidget* w);

    SessionState*     m_state;
    BluetoothManager* m_btManager;
    QMap<QString, Category> m_categories;

    BluetoothPanel* m_bluetoothPanel;
    QScrollArea*    m_rightScroll;
    StepFlow*       m_stepFlow;

    QWidget*    m_caliberBlock;
    QWidget*    m_profileBlock;
    QWidget*    m_distanceBlock;
    QWidget*    m_drillBlock;
    QWidget*    m_actionBlock;

    ButtonGrid* m_categoryGrid;
    ButtonGrid* m_caliberGrid;
    ButtonGrid* m_profileGrid;
    ButtonGrid* m_distanceGrid;
    ButtonGrid* m_drillGrid;

    QPushButton* m_resetBtn;
    QPushButton* m_nextBtn;

    QLabel* m_badgeCategory;
    QLabel* m_badgeCaliber;
    QLabel* m_badgeProfile;
    QLabel* m_badgeDistance;
};