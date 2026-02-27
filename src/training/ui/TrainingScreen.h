#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QScrollArea>
#include "core/SessionState.h"
#include "training/engine/dart_system.h"

/**
 * @brief Active training screen.
 *
 * Layout  :  camera feed (left 60%) | control panel (right 40%)
 *
 * Right panel phases:
 *   Framing     → Loading button until first frame arrives → Frame Target
 *   Calibrating → mode_calibration, shows Stop + Start Scoring
 *   Scoring     → confirm_calibration succeeded, shows shot grid
 *
 * BLE or camera disconnect at any time → stopAndExit().
 */
class TrainingScreen : public QWidget {
    Q_OBJECT
public:
    explicit TrainingScreen(SessionState* state, QWidget* parent = nullptr);

    /** Navigate here first, then call this to init engine + start timer. */
    void beginSession();

signals:
    void sessionEnded();

private:
    enum Phase { Framing, Calibrating, Scoring };

    void buildUI();
    QWidget* buildGuidePanel();
    QWidget* buildScoringPanel();

    void enterCalibrating();
    void enterScoring();
    void stopAndExit();
    void onTick();
    void addShotRow(int num, int score);

    SessionState*   m_state;

    // Left
    QLabel*         m_cameraView;

    // Right
    QStackedWidget* m_rightStack;      // 0=guide, 1=scoring
    QStackedWidget* m_guideBottom;     // 0=loading, 1=frameTarget, 2=stop+startScoring
    QWidget*        m_step4;
    QVBoxLayout*    m_shotsLayout;
    QLabel*         m_totalScore;
    QPushButton*    m_pauseBtn;

    QTimer*         m_tickTimer;
    Phase           m_phase       = Framing;
    int             m_currentMode = MODE_STREAMING;
    bool            m_paused      = false;
    bool            m_camReady    = false;
    int             m_shotCount   = 0;
};