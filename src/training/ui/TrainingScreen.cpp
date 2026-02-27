#include "TrainingScreen.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QHBoxLayout>
#include <QDateTime>
#include <QStandardPaths>
#include <opencv2/opencv.hpp>

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────

TrainingScreen::TrainingScreen(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    buildUI();

    // Guard: disconnect mid-session → stop everything
    auto onDisconnect = [this](bool ok) { if (!ok) stopAndExit(); };
    connect(m_state, &SessionState::bluetoothConnectionChanged, this, onDisconnect);
    connect(m_state, &SessionState::cameraConnectionChanged,   this, onDisconnect);

    m_frameTimer = new QTimer(this);
    m_frameTimer->setInterval(33);   // ~30 fps
    connect(m_frameTimer, &QTimer::timeout, this, &TrainingScreen::pollFrame);

    m_dataTimer = new QTimer(this);
    m_dataTimer->setInterval(300);
    connect(m_dataTimer, &QTimer::timeout, this, &TrainingScreen::pollData);
}

void TrainingScreen::beginSession() {
    start();
    mode_streaming();
    m_frameTimer->start();
}

// ─────────────────────────────────────────────────────────────────────────────
//  UI construction helpers
// ─────────────────────────────────────────────────────────────────────────────

/** Builds a top bar.
 *  @param showDownload  true = download icon (scoring), false = back arrow (guide) */
static QWidget* makeTopBar(bool showDownload,
                            const std::function<void()>& leftAction,
                            const std::function<void()>& downloadAction = {})
{
    auto* bar = new QWidget;
    bar->setStyleSheet(AppTheme::transparent());
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(14, 10, 14, 10);

    if (showDownload) {
        auto* btn = new QPushButton("⬇", bar);
        btn->setStyleSheet(AppTheme::buttonGhost());
        btn->setFixedSize(34, 34);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip("Save screenshot");
        QObject::connect(btn, &QPushButton::clicked, [downloadAction]{ if (downloadAction) downloadAction(); });
        l->addWidget(btn);
    } else {
        auto* btn = new QPushButton("←", bar);
        btn->setStyleSheet(AppTheme::buttonGhost());
        btn->setFixedSize(34, 34);
        btn->setCursor(Qt::PointingHandCursor);
        QObject::connect(btn, &QPushButton::clicked, [leftAction]{ leftAction(); });
        l->addWidget(btn);
    }

    l->addStretch();

    // Connection indicators
    for (const auto& text : {"⊛  Wi-Fi", "⬡  BLE"}) {
        auto* icon = new QLabel(text, bar);
        icon->setStyleSheet(AppTheme::connectedIcon());
        l->addWidget(icon);
        l->addSpacing(6);
    }

    bar->setLayout(l);
    return bar;
}

/** Adds a labelled step block to a layout. Returns the root widget. */
static QWidget* makeStep(QWidget* parent, const QString& num, const QString& title,
                          const QStringList& bullets)
{
    auto* block = new QWidget(parent);
    block->setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(block);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(4);

    auto* heading = new QLabel(num + ". " + title, block);
    heading->setStyleSheet(AppTheme::instructionStep());
    vb->addWidget(heading);

    for (const auto& b : bullets) {
        auto* lbl = new QLabel("  " + b, block);
        lbl->setStyleSheet(AppTheme::instructionItem());
        lbl->setWordWrap(true);
        vb->addWidget(lbl);
    }
    block->setLayout(vb);
    return block;
}

void TrainingScreen::buildUI() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left: camera feed ─────────────────────────────────────────────────
    m_cameraView = new QLabel(this);
    m_cameraView->setAlignment(Qt::AlignCenter);
    m_cameraView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_cameraView->setStyleSheet(AppTheme::cameraView());
    m_cameraView->setText("Connecting to camera...");

    // ── Right: guide + scoring pages ──────────────────────────────────────
    m_rightStack = new QStackedWidget(this);
    m_rightStack->addWidget(buildGuidePanel());   // 0
    m_rightStack->addWidget(buildScoringPanel()); // 1

    root->addWidget(m_cameraView,    60);
    root->addWidget(m_rightStack,    40);
    setLayout(root);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Guide panel (Framing + Calibrating)
// ─────────────────────────────────────────────────────────────────────────────

QWidget* TrainingScreen::buildGuidePanel() {
    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    // Top bar
    vb->addWidget(makeTopBar(false, [this]{ stopAndExit(); }));

    // Instructions scroll
    auto* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());

    auto* content = new QWidget;
    auto* cl = new QVBoxLayout(content);
    cl->setContentsMargins(18, 16, 18, 16);
    cl->setSpacing(16);

    auto* guideTitle = new QLabel("Realtime Scoring Guide", content);
    guideTitle->setStyleSheet(AppTheme::sectionTitle());
    guideTitle->setAlignment(Qt::AlignCenter);
    cl->addWidget(guideTitle);

    cl->addWidget(makeStep(content, "1", "Start", {
        "• Attach AimSync BLE device to your finger.",
        "• Press Start to begin detection, press again to start scoring.",
        "• After the start beep, fire the first shot."
    }));
    cl->addWidget(makeStep(content, "2", "During Shooting", {
        "• Beep = shot detected → fire the next shot.",
        "• No beep = shot missed (score 0).",
        "• Always wait for the beep before firing again."
    }));
    cl->addWidget(makeStep(content, "3", "Pause / Resume", {
        "• Tap Pause anytime.",
        "• Tap Resume to continue."
    }));

    // Step 4 is hidden until Calibrating phase
    m_step4 = makeStep(content, "4", "Finish", {
        "• Session automatically finishes after all shots."
    });
    m_step4->hide();
    cl->addWidget(m_step4);
    cl->addStretch();

    content->setLayout(cl);
    scroll->setWidget(content);
    vb->addWidget(scroll, 1);

    // ── Bottom: 3-state mini-stack ────────────────────────────────────────
    m_guideBottom = new QStackedWidget(panel);

    // 0 — Loading (camera not ready yet)
    auto* loadingPage = new QWidget;
    auto* ll = new QHBoxLayout(loadingPage);
    ll->setContentsMargins(16, 10, 16, 16);
    ll->addStretch();
    auto* loadBtn = new QPushButton("Loading...", loadingPage);
    loadBtn->setStyleSheet(AppTheme::buttonPrimary());
    loadBtn->setEnabled(false);
    ll->addWidget(loadBtn);
    loadingPage->setLayout(ll);

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
    auto* twoBtnPage = new QWidget;
    auto* tl = new QHBoxLayout(twoBtnPage);
    tl->setContentsMargins(16, 10, 16, 16);
    tl->addStretch();
    auto* stopBtn = new QPushButton("Stop", twoBtnPage);
    stopBtn->setStyleSheet(AppTheme::buttonDanger());
    stopBtn->setCursor(Qt::PointingHandCursor);
    connect(stopBtn, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);
    auto* startScoringBtn = new QPushButton("Start Scoring", twoBtnPage);
    startScoringBtn->setStyleSheet(AppTheme::buttonPrimary());
    startScoringBtn->setCursor(Qt::PointingHandCursor);
    connect(startScoringBtn, &QPushButton::clicked, this, &TrainingScreen::enterScoring);
    tl->addWidget(stopBtn);
    tl->addSpacing(8);
    tl->addWidget(startScoringBtn);
    twoBtnPage->setLayout(tl);

    m_guideBottom->addWidget(loadingPage);   // 0
    m_guideBottom->addWidget(framePage);     // 1
    m_guideBottom->addWidget(twoBtnPage);    // 2
    m_guideBottom->setCurrentIndex(0);

    vb->addWidget(m_guideBottom);
    panel->setLayout(vb);
    return panel;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Scoring panel
// ─────────────────────────────────────────────────────────────────────────────

QWidget* TrainingScreen::buildScoringPanel() {
    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    // Top bar (download screenshot)
    vb->addWidget(makeTopBar(true, {}, [this] {
        QPixmap px = grab();
        QString path = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
                       + "/session_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";
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

    // Shot rows (scroll)
    auto* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());

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
    auto* scoreLbl = new QLabel("TOTAL SCORE", scoreBox);
    scoreLbl->setStyleSheet(AppTheme::labelMuted());
    m_totalScore = new QLabel("0", scoreBox);
    m_totalScore->setStyleSheet(AppTheme::scoreValue());
    sl->addWidget(scoreLbl);
    sl->addWidget(m_totalScore);
    scoreBox->setLayout(sl);
    vb->addWidget(scoreBox);

    // Bottom: Pause | End Session
    auto* bottomBar = new QWidget(panel);
    bottomBar->setStyleSheet(AppTheme::transparent());
    auto* bl = new QHBoxLayout(bottomBar);
    bl->setContentsMargins(16, 10, 16, 16);

    m_pauseBtn = new QPushButton("⏸  Pause", bottomBar);
    m_pauseBtn->setStyleSheet(AppTheme::buttonDanger());
    m_pauseBtn->setCursor(Qt::PointingHandCursor);
    connect(m_pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_paused) {
            resume();
            m_paused = false;
            m_pauseBtn->setText("⏸  Pause");
            m_dataTimer->start();
        } else {
            pause();
            m_paused = true;
            m_pauseBtn->setText("▶  Resume");
            m_dataTimer->stop();
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
    mode_calibration();
    m_phase = Calibrating;
    m_step4->show();
    m_guideBottom->setCurrentIndex(2);  // show Stop + Start Scoring
}

void TrainingScreen::enterScoring() {
    if (!confirm()) {
        SnackBar::show(window(),
            "Calibration not confirmed. Adjust framing and try again.",
            SnackBar::Warning);
        return;
    }
    m_phase = Scoring;
    m_rightStack->setCurrentIndex(1);
    m_dataTimer->start();
}

void TrainingScreen::stopAndExit() {
    m_frameTimer->stop();
    m_dataTimer->stop();
    stop();
    emit sessionEnded();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Timers
// ─────────────────────────────────────────────────────────────────────────────

void TrainingScreen::pollFrame() {
    // NOTE: adjust get_frame() to match the actual dart_system.h function name
    cv::Mat frame = get_frame();
    if (frame.empty()) return;

    QImage img(frame.data, frame.cols, frame.rows,
               (int)frame.step, QImage::Format_BGR888);
    m_cameraView->setPixmap(
        QPixmap::fromImage(img).scaled(m_cameraView->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));

    if (!m_camReady) {
        m_camReady = true;
        if (m_phase == Framing)
            m_guideBottom->setCurrentIndex(1);  // Loading → Frame Target
    }
}

void TrainingScreen::pollData() {
    // NOTE: adjust get_shot_count() / get_shot_score() / get_total_score()
    // to match the actual dart_system.h API.
    int newCount = get_shot_count();
    for (int i = m_shotCount; i < newCount; ++i) {
        ++m_shotCount;
        addShotRow(m_shotCount, get_shot_score(m_shotCount));
    }
    m_totalScore->setText(QString::number(get_total_score()));
}

void TrainingScreen::addShotRow(int num, int score) {
    auto* row = new QWidget;
    row->setStyleSheet(AppTheme::gridDataRow());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(16, 10, 16, 10);

    auto makeCell = [&](const QString& text) {
        auto* lbl = new QLabel(text, row);
        lbl->setStyleSheet(AppTheme::gridDataCell());
        lbl->setAlignment(Qt::AlignCenter);
        rl->addWidget(lbl, 1);
    };

    makeCell(QString::number(num));
    makeCell(QString::number(score));
    makeCell("--");   // split time: not yet available
    makeCell("●");    // direction: not yet available
    row->setLayout(rl);

    // Insert before the trailing stretch
    m_shotsLayout->insertWidget(m_shotsLayout->count() - 1, row);
}
