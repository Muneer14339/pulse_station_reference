#pragma once
// src/training/ui/TrainingScreen.h
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDateTime>
#include "core/SessionState.h"
#include "training/engine/dart_system.h"
#include "training/data/ReviewDataTypes.h"
#include "ui/widgets/ShotGridWidget.h"         // ← shared widget

/**
 * @brief Active training screen.
 *
 * Layout: camera feed (left 60%) | control panel (right 40%)
 *
 * Right panel phases:
 *   Framing     → Loading until first frame → Frame Target
 *   Calibrating → mode_calibration, shows Stop + Start Scoring
 *   Scoring     → shot grid (ShotGridWidget — same as ShotCountTab in review)
 *
 * BLE or camera disconnect → stopAndExit().
 * Session end → emits sessionEnded(SessionResult) with live shot data.
 */
class TrainingScreen : public QWidget {
    Q_OBJECT
public:
    explicit TrainingScreen(SessionState* state, QWidget* parent = nullptr);

    /** Navigate to this screen first, then call beginSession(). */
    void beginSession();

signals:
    void sessionEnded(const SessionResult& result);

private:
    enum Phase { Framing, Calibrating, Scoring };

    void buildUI();
    QWidget* buildGuidePanel();
    QWidget* buildScoringPanel();

    void enterCalibrating();
    void enterScoring();
    void stopAndExit();
    void onTick();

    SessionState*   m_state;

    // Left
    QLabel*         m_cameraView;

    // Right
    QStackedWidget* m_rightStack;       // 0=guide, 1=scoring
    QStackedWidget* m_guideBottom;      // 0=loading, 1=frameTarget, 2=stop+startScoring
    QWidget*        m_step4;

    // Scoring panel (right side, index 1)
    ShotGridWidget* m_shotGrid;         // ← shared with ShotCountTab
    QLabel*         m_totalScore;
    QPushButton*    m_pauseBtn;

    QTimer*  m_tickTimer;
    Phase    m_phase       = Framing;
    int      m_currentMode = MODE_STREAMING;
    bool     m_paused      = false;
    bool     m_camReady    = false;

    // Accumulated live data → packaged into SessionResult on exit
    QVector<ShotRecord> m_shotRecords;
    QDateTime           m_sessionStart;
};