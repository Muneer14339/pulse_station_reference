#pragma once
#include <QWidget>
#include <QPushButton>
#include "core/SessionState.h"
#include "ui/widgets/SummaryBox.h"
#include "training/ui/CameraPanel.h"

/**
 * @brief Training session screen (Screen 3).
 * Left sidebar: CameraPanel (mirrors BluetoothPanel layout).
 * Right: session summary + Start Session button (enabled only when
 * both BLE and camera are connected).
 */
class TrainingPlaceholder : public QWidget {
    Q_OBJECT
public:
    explicit TrainingPlaceholder(SessionState* state, QWidget* parent = nullptr);
    void refresh();

signals:
    void newSessionRequested();
    void sessionStarted();      ///< Navigate to TrainingScreen, then call beginSession()

private:
    void buildUI();
    void updateStartButton();

    SessionState* m_state;
    SummaryBox*   m_summaryBox;
    QPushButton*  m_startBtn;
    CameraPanel*  m_cameraPanel;
};