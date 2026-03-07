#pragma once
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDateTime>
#include "core/SessionState.h"
#include "core/BluetoothManager.h"
#include "core/ShotCorrelator.h"
#include "training/engine/dart_system.h"
#include "training/data/ReviewDataTypes.h"
#include "ui/widgets/ShotGridWidget.h"

class TrainingScreen : public QWidget {
    Q_OBJECT
public:
    explicit TrainingScreen(SessionState*     state,
                            BluetoothManager* btManager,
                            QWidget*          parent = nullptr);
    void beginSession();

signals:
    void sessionEnded(const SessionResult& result);
    void backRequested();

private:
    enum Phase { Framing, Calibrating, Scoring };

    void buildUI();
    QWidget* buildGuidePanel();
    QWidget* buildScoringPanel();

    void enterCalibrating();
    void enterScoring();
    void stopAndExit();
    void cancelSession();
    void onTick();
    void onShotFinalized(const ShotRecord& rec);

    SessionState*     m_state;
    BluetoothManager* m_btManager;
    ShotCorrelator*   m_correlator  = nullptr;

    QLabel*         m_cameraView;
    QStackedWidget* m_rightStack;
    QStackedWidget* m_guideBottom;
    QWidget*        m_step4;
    ShotGridWidget* m_shotGrid;
    QLabel*         m_totalScore;
    QPushButton*    m_pauseBtn;

    QTimer*  m_tickTimer;
    Phase    m_phase       = Framing;
    int      m_currentMode = MODE_STREAMING;
    bool     m_paused      = false;
    bool     m_camReady    = false;

    QVector<ShotRecord> m_shotRecords;
    QDateTime           m_sessionStart;
};