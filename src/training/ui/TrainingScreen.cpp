#include "TrainingScreen.h"
#include "training/data/ScoringGuideData.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QHBoxLayout>
#include <QDateTime>
#include <QStandardPaths>

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────

TrainingScreen::TrainingScreen(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    buildUI();

    // Guard: any disconnect mid-session → stop and go back
    auto onDisconnect = [this](bool ok) { if (!ok) stopAndExit(); };
    connect(m_state, &SessionState::bluetoothConnectionChanged, this, onDisconnect);
    connect(m_state, &SessionState::cameraConnectionChanged,   this, onDisconnect);

    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(33);   // ~30 fps
    connect(m_tickTimer, &QTimer::timeout, this, &TrainingScreen::onTick);
}

void TrainingScreen::beginSession() {
    // Reset internal state for re-entry
    m_phase       = Framing;
    m_paused      = false;
    m_camReady    = false;
    m_shotCount   = 0;
    m_currentMode = MODE_STREAMING;
    m_rightStack->setCurrentIndex(0);
    m_guideBottom->setCurrentIndex(0);
    m_step4->hide();

    init_system(m_state->cameraIndex(), 600);
    m_tickTimer->start();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Shared UI helpers
// ─────────────────────────────────────────────────────────────────────────────

static QWidget* makeTopBar(bool showDownload,
                            QObject* ctx,
                            const std::function<void()>& leftAction,
                            const std::function<void()>& downloadAction = {})
{
    auto* bar = new QWidget;
    bar->setStyleSheet(AppTheme::transparent());
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(18, 16, 18, 12);

    auto* leftBtn = new QPushButton(showDownload ? "⬇" : "←", bar);
    leftBtn->setStyleSheet(AppTheme::refreshButton());
    leftBtn->setFixedSize(36, 36);
    leftBtn->setCursor(Qt::PointingHandCursor);
    if (showDownload)
        QObject::connect(leftBtn, &QPushButton::clicked, ctx,
                         [downloadAction]{ if (downloadAction) downloadAction(); });
    else
        QObject::connect(leftBtn, &QPushButton::clicked, ctx,
                         [leftAction]{ leftAction(); });
    l->addWidget(leftBtn);
    l->addStretch();

    auto* wifiLbl = new QLabel("● Wi-Fi", bar);
    wifiLbl->setStyleSheet(AppTheme::connectedIcon());
    auto* bleLbl  = new QLabel("⬡  BLE", bar);
    bleLbl->setStyleSheet(AppTheme::connectedIcon());
    l->addWidget(wifiLbl);
    l->addSpacing(14);
    l->addWidget(bleLbl);

    bar->setLayout(l);
    return bar;
}

static QWidget* makeStep(QWidget* parent, const GuideStep& step) {
    auto* block = new QWidget(parent);
    block->setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(block);
    vb->setContentsMargins(0, 0, 0, 8);
    vb->setSpacing(6);
    auto* heading = new QLabel(step.title, block);
    heading->setStyleSheet(AppTheme::helpTitle());
    vb->addWidget(heading);
    for (const auto& b : step.bullets) {
        auto* lbl = new QLabel(b, block);
        lbl->setStyleSheet(AppTheme::helpItem());
        lbl->setWordWrap(true);
        vb->addWidget(lbl);
    }
    block->setLayout(vb);
    return block;
}

// ─────────────────────────────────────────────────────────────────────────────
//  UI construction
// ─────────────────────────────────────────────────────────────────────────────

void TrainingScreen::buildUI() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left: live camera feed ────────────────────────────────────────────
    m_cameraView = new QLabel(this);
    m_cameraView->setAlignment(Qt::AlignCenter);
    m_cameraView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_cameraView->setStyleSheet(AppTheme::cameraView());
    m_cameraView->setText("Connecting to camera...");

    // ── Right: guide (0) / scoring (1) ───────────────────────────────────
    m_rightStack = new QStackedWidget(this);
    m_rightStack->addWidget(buildGuidePanel());
    m_rightStack->addWidget(buildScoringPanel());

    root->addWidget(m_cameraView,  60);
    root->addWidget(m_rightStack,  40);
    setLayout(root);
}

QWidget* TrainingScreen::buildGuidePanel() {
    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    vb->addWidget(makeTopBar(false, this, [this]{ stopAndExit(); }));

    auto* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::transparent());

    auto* content = new QWidget;
    content->setStyleSheet(AppTheme::transparent());
    auto* cl = new QVBoxLayout(content);
    cl->setContentsMargins(18, 16, 18, 16);
    cl->setSpacing(16);

    auto* guideTitle = new QLabel("Realtime Scoring Guide", content);
    guideTitle->setStyleSheet(AppTheme::sectionTitle());
    guideTitle->setAlignment(Qt::AlignCenter);
    cl->addWidget(guideTitle);

    for (const auto& step : scoringGuideSteps())
        cl->addWidget(makeStep(content, step));

    m_step4 = makeStep(content, scoringGuideStep4());
    m_step4->hide();
    cl->addWidget(m_step4);
    cl->addStretch();
    content->setLayout(cl);
    scroll->setWidget(content);
    vb->addWidget(scroll, 1);

    // ── Bottom 3-state: Loading (0) | FrameTarget (1) | Stop+StartScoring (2)
    m_guideBottom = new QStackedWidget(panel);
    m_guideBottom->setStyleSheet(AppTheme::transparent());

    // 0 — Loading
    auto* loadPage = new QWidget;
    auto* ll = new QHBoxLayout(loadPage);
    ll->setContentsMargins(16, 10, 16, 16);
    ll->addStretch();
    auto* loadBtn = new QPushButton("Loading...", loadPage);
    loadBtn->setStyleSheet(AppTheme::buttonPrimary());
    loadBtn->setEnabled(false);
    ll->addWidget(loadBtn);
    loadPage->setLayout(ll);

    // 1 — Frame Target
    auto* framePage = new QWidget;
    auto* fl = new QHBoxLayout(framePage);
    fl->setContentsMargins(16, 10, 16, 16);
    fl->addStretch();
    auto* frameBtn = new QPushButton("Frame Target", framePage);
    frameBtn->setStyleSheet(AppTheme::buttonPrimary());
    frameBtn->setCursor(Qt::PointingHandCursor);
    connect(frameBtn, &QPushButton::clicked, this, &TrainingScreen::enterCalibrating);
    fl->addWidget(frameBtn);
    framePage->setLayout(fl);

    // 2 — Stop + Start Scoring
    auto* twoPage = new QWidget;
    auto* tl = new QHBoxLayout(twoPage);
    tl->setContentsMargins(16, 10, 16, 16);
    tl->addStretch();
    auto* stopBtn = new QPushButton("Stop", twoPage);
    stopBtn->setStyleSheet(AppTheme::buttonDanger());
    stopBtn->setCursor(Qt::PointingHandCursor);
    connect(stopBtn, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);
    auto* scoringBtn = new QPushButton("Start Scoring", twoPage);
    scoringBtn->setStyleSheet(AppTheme::buttonPrimary());
    scoringBtn->setCursor(Qt::PointingHandCursor);
    connect(scoringBtn, &QPushButton::clicked, this, &TrainingScreen::enterScoring);
    tl->addWidget(stopBtn);
    tl->addSpacing(8);
    tl->addWidget(scoringBtn);
    twoPage->setLayout(tl);

    m_guideBottom->addWidget(loadPage);   // 0
    m_guideBottom->addWidget(framePage);  // 1
    m_guideBottom->addWidget(twoPage);    // 2

    vb->addWidget(m_guideBottom);
    panel->setLayout(vb);
    return panel;
}

QWidget* TrainingScreen::buildScoringPanel() {
    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    vb->addWidget(makeTopBar(true, this, {}, [this] {
        QPixmap px = grab();
        QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                       + "/session_"
                       + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")
                       + ".png";
        px.save(path);
        SnackBar::show(window(), "Screenshot saved: " + path, SnackBar::Success);
    }));

    // Grid header
    auto* header = new QWidget(panel);
    header->setStyleSheet(AppTheme::gridHeader());
    auto* hl = new QHBoxLayout(header);
    hl->setContentsMargins(16, 8, 16, 8);
    for (const auto* col : {"Shot #", "Score", "Split Time", "Direction"}) {
        auto* lbl = new QLabel(col, header);
        lbl->setStyleSheet(AppTheme::gridHeaderCell());
        lbl->setAlignment(Qt::AlignCenter);
        hl->addWidget(lbl, 1);
    }
    header->setLayout(hl);
    vb->addWidget(header);

    // Shot rows (scrollable)
    auto* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::transparent());
    auto* shotContainer = new QWidget;
    m_shotsLayout = new QVBoxLayout(shotContainer);
    m_shotsLayout->setContentsMargins(0, 0, 0, 0);
    m_shotsLayout->setSpacing(0);
    m_shotsLayout->addStretch();
    shotContainer->setLayout(m_shotsLayout);
    scroll->setWidget(shotContainer);
    vb->addWidget(scroll, 1);

    // Total score box
    auto* scoreBox = new QWidget(panel);
    scoreBox->setStyleSheet(AppTheme::scorePanel());
    auto* sl = new QVBoxLayout(scoreBox);
    sl->setContentsMargins(20, 14, 20, 14);
    sl->setSpacing(2);
    auto* scoreTitleLbl = new QLabel("TOTAL SCORE", scoreBox);
    scoreTitleLbl->setStyleSheet(AppTheme::labelMuted());
    sl->addWidget(scoreTitleLbl);

    m_totalScore = new QLabel("0", scoreBox);
    m_totalScore->setStyleSheet(AppTheme::scoreValue());

    sl->addWidget(m_totalScore);
    scoreBox->setLayout(sl);
    vb->addWidget(scoreBox);

    // Pause + End Session
    auto* bottomBar = new QWidget(panel);
    bottomBar->setStyleSheet(AppTheme::transparent());
    auto* bl = new QHBoxLayout(bottomBar);
    bl->setContentsMargins(16, 10, 16, 16);

    m_pauseBtn = new QPushButton("⏸  Pause", bottomBar);
    m_pauseBtn->setStyleSheet(AppTheme::buttonDanger());
    m_pauseBtn->setCursor(Qt::PointingHandCursor);
    connect(m_pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_paused) {
            resume_system();
            m_paused = false;
            m_pauseBtn->setText("⏸  Pause");
        } else {
            pause_system();
            m_paused = true;
            m_pauseBtn->setText("▶  Resume");
        }
    });

    auto* endBtn = new QPushButton("End Session", bottomBar);
    endBtn->setStyleSheet(AppTheme::buttonPrimary());
    endBtn->setCursor(Qt::PointingHandCursor);
    connect(endBtn, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);

    bl->addWidget(m_pauseBtn);
    bl->addStretch();
    bl->addWidget(endBtn);
    bottomBar->setLayout(bl);
    vb->addWidget(bottomBar);

    panel->setLayout(vb);
    return panel;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Phase transitions
// ─────────────────────────────────────────────────────────────────────────────

void TrainingScreen::enterCalibrating() {
    m_currentMode = MODE_CALIBRATION;
    m_phase       = Calibrating;
    m_step4->show();
    m_guideBottom->setCurrentIndex(2);
}

void TrainingScreen::enterScoring() {
    if (!confirm_calibration()) {
        SnackBar::show(window(),
            "Calibration not confirmed. Adjust framing and try again.",
            SnackBar::Error);
        return;
    }
    m_currentMode = MODE_SCORING;
    m_phase       = Scoring;
    m_rightStack->setCurrentIndex(1);
}

void TrainingScreen::stopAndExit() {
    m_tickTimer->stop();
    cleanup_system();
    emit sessionEnded();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tick — processes one frame, updates feed and shot data
// ─────────────────────────────────────────────────────────────────────────────

void TrainingScreen::onTick() {
    if (m_paused) return;

    FrameResult result = process_frame(m_currentMode);

    // Update camera view
    if (!result.frame.empty())
    {
        // Convert BGR (OpenCV default) → RGB (Qt expects)
        cv::Mat rgbFrame;
        cv::cvtColor(result.frame, rgbFrame, cv::COLOR_BGR2RGB);

        QImage img(
            rgbFrame.data,
            rgbFrame.cols,
            rgbFrame.rows,
            static_cast<int>(rgbFrame.step),
            QImage::Format_RGB888
        );

        // Copy image to make it safe (prevents memory issues)
        m_cameraView->setPixmap(
            QPixmap::fromImage(img.copy()).scaled(
                m_cameraView->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        );

        // Camera ready → show Frame Target button
        if (!m_camReady && m_phase == Framing)
        {
            m_camReady = true;
            m_guideBottom->setCurrentIndex(1);  // Loading → Frame Target
        }
    }

    // New shot in scoring phase
    if (m_phase == Scoring && result.has_new_shot) {
        ++m_shotCount;
        addShotRow(result.shot.shot_number, result.shot.score);
        m_totalScore->setText(QString::number(result.shot.total_score));
    }
}

void TrainingScreen::addShotRow(int num, int score) {
    auto* row = new QWidget;
    row->setStyleSheet(AppTheme::gridDataRow());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(16, 10, 16, 10);
    for (const auto& text : {QString::number(num), QString::number(score),
                              QStringLiteral("--"), QStringLiteral("●")}) {
        auto* lbl = new QLabel(text, row);
        lbl->setStyleSheet(AppTheme::gridDataCell());
        lbl->setAlignment(Qt::AlignCenter);
        rl->addWidget(lbl, 1);
    }
    row->setLayout(rl);
    m_shotsLayout->insertWidget(m_shotsLayout->count() - 1, row);
}