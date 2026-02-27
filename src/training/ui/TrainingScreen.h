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
 * Layout: camera feed (left 60%) | control panel (right 40%)
 *
 * Right panel cycles through 3 phases:
 *   Framing   → camera connecting, shows Loading button
 *   Calibrating → mode_calibration() called, shows Stop + Start Scoring
 *   Scoring   → confirm() succeeded, shows shot grid + score
 *
 * Any BLE/camera disconnect during a session → stop() + sessionEnded().
 */
class TrainingScreen : public QWidget {
    Q_OBJECT
public:
    explicit TrainingScreen(SessionState* state, QWidget* parent = nullptr);

    /** Called by MainWindow after navigating here.
     *  Calls start() → mode_streaming(), starts frame polling. */
    void beginSession();

signals:
    void sessionEnded();   ///< Navigate back to TrainingPlaceholder

private:
    enum Phase { Framing, Calibrating, Scoring };

    void buildUI();
    QWidget* buildGuidePanel();
    QWidget* buildScoringPanel();

    void enterCalibrating();
    void enterScoring();
    void stopAndExit();
    void pollFrame();
    void pollData();
    void addShotRow(int num, int score);

    SessionState*   m_state;

    // Left — camera feed
    QLabel*         m_cameraView;

    // Right — phase switcher
    QStackedWidget* m_rightStack;      // 0=guide, 1=scoring

    // Guide panel internals
    QStackedWidget* m_guideBottom;     // 0=loading 1=frameTarget 2=stop+startScoring
    QWidget*        m_step4;           // "4. Finish" block — hidden until Calibrating

    // Scoring panel internals
    QVBoxLayout*    m_shotsLayout;
    QLabel*         m_totalScore;
    QPushButton*    m_pauseBtn;

    QTimer*         m_frameTimer;
    QTimer*         m_dataTimer;
    Phase           m_phase    = Framing;
    bool            m_paused   = false;
    bool            m_camReady = false;
    int             m_shotCount= 0;
};
