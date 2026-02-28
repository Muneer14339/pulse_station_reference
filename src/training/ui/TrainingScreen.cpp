// src/training/ui/TrainingScreen.cpp
#include "TrainingScreen.h"
#include "training/data/ScoringGuideData.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QHBoxLayout>
#include <QStandardPaths>

TrainingScreen::TrainingScreen(SessionState* state, QWidget* parent)
    : QWidget(parent), m_state(state)
{
    buildUI();
    auto onDisconnect = [this](bool ok) { if (!ok) stopAndExit(); };
    connect(m_state, &SessionState::bluetoothConnectionChanged, this, onDisconnect);
    connect(m_state, &SessionState::cameraConnectionChanged,   this, onDisconnect);
    m_tickTimer = new QTimer(this);
    m_tickTimer->setInterval(33);
    connect(m_tickTimer, &QTimer::timeout, this, &TrainingScreen::onTick);
}

void TrainingScreen::beginSession() {
    m_phase       = Framing;
    m_paused      = false;
    m_camReady    = false;
    m_currentMode = MODE_STREAMING;
    m_shotRecords.clear();
    m_sessionStart = QDateTime::currentDateTime();
    m_shotGrid->clear();
    m_totalScore->setText("0");
    m_rightStack->setCurrentIndex(0);
    m_guideBottom->setCurrentIndex(0);
    m_step4->hide();
    init_system(m_state->cameraIndex(), 600);
    m_tickTimer->start();
}

static QWidget* makeTopBar(bool showDownload, QObject* ctx,
                            const std::function<void()>& leftAction,
                            const std::function<void()>& downloadAction = {})
{
    auto* bar = new QWidget;
    bar->setStyleSheet(AppTheme::transparent());
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(18, 16, 18, 12);

    auto* btn = new QPushButton(showDownload ? "⬇" : "←", bar);
    btn->setStyleSheet(AppTheme::refreshButton());
    btn->setFixedSize(36, 36);
    btn->setCursor(Qt::PointingHandCursor);
    if (showDownload)
        QObject::connect(btn, &QPushButton::clicked, ctx,
                         [downloadAction]{ if (downloadAction) downloadAction(); });
    else
        QObject::connect(btn, &QPushButton::clicked, ctx,
                         [leftAction]{ leftAction(); });
    l->addWidget(btn);
    l->addStretch();

    for (const char* txt : {"● Wi-Fi", "⬡  BLE"}) {
        auto* lbl = new QLabel(txt, bar);
        lbl->setStyleSheet(AppTheme::connectedIcon());
        l->addWidget(lbl);
        if (txt == std::string("● Wi-Fi")) l->addSpacing(14);
    }
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

void TrainingScreen::buildUI() {
    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_cameraView = new QLabel(this);
    m_cameraView->setAlignment(Qt::AlignCenter);
    m_cameraView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_cameraView->setStyleSheet(AppTheme::cameraView());
    m_cameraView->setText("Connecting to camera...");

    m_rightStack = new QStackedWidget(this);
    m_rightStack->addWidget(buildGuidePanel());
    m_rightStack->addWidget(buildScoringPanel());

    root->addWidget(m_cameraView, 60);
    root->addWidget(m_rightStack, 40);
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

    m_guideBottom = new QStackedWidget(panel);
    m_guideBottom->setStyleSheet(AppTheme::transparent());

    // 0 — Loading
    { auto* pg = new QWidget; auto* l = new QHBoxLayout(pg);
      l->setContentsMargins(18, 12, 18, 18); l->addStretch();
      auto* b = new QPushButton("Loading...", pg);
      b->setStyleSheet(AppTheme::buttonPrimary()); b->setEnabled(false);
      l->addWidget(b); pg->setLayout(l); m_guideBottom->addWidget(pg); }

    // 1 — Frame Target
    { auto* pg = new QWidget; auto* l = new QHBoxLayout(pg);
      l->setContentsMargins(18, 12, 18, 18); l->addStretch();
      auto* b = new QPushButton("Frame Target", pg);
      b->setStyleSheet(AppTheme::buttonPrimary());
      b->setCursor(Qt::PointingHandCursor);
      connect(b, &QPushButton::clicked, this, &TrainingScreen::enterCalibrating);
      l->addWidget(b); pg->setLayout(l); m_guideBottom->addWidget(pg); }

    // 2 — Stop + Start Scoring
    { auto* pg = new QWidget; auto* l = new QHBoxLayout(pg);
      l->setContentsMargins(18, 12, 18, 18); l->addStretch();
      auto* stop = new QPushButton("Stop", pg);
      stop->setStyleSheet(AppTheme::buttonDanger());
      stop->setCursor(Qt::PointingHandCursor);
      connect(stop, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);
      auto* sc = new QPushButton("Start Scoring", pg);
      sc->setStyleSheet(AppTheme::buttonPrimary());
      sc->setCursor(Qt::PointingHandCursor);
      connect(sc, &QPushButton::clicked, this, &TrainingScreen::enterScoring);
      l->addWidget(stop); l->addSpacing(10); l->addWidget(sc);
      pg->setLayout(l); m_guideBottom->addWidget(pg); }

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

    // Grid with inner margin
    auto* gridWrapper = new QWidget(panel);
    gridWrapper->setStyleSheet(AppTheme::transparent());
    auto* gw = new QVBoxLayout(gridWrapper);
    gw->setContentsMargins(12, 8, 12, 8);
    gw->setSpacing(0);
    m_shotGrid = new ShotGridWidget(gridWrapper);
    gw->addWidget(m_shotGrid);
    gridWrapper->setLayout(gw);
    vb->addWidget(gridWrapper, 1);

    // Total score panel
    auto* scoreBox = new QWidget(panel);
    scoreBox->setStyleSheet(AppTheme::scorePanel());
    auto* sl = new QVBoxLayout(scoreBox);
    sl->setContentsMargins(20, 14, 20, 14);
    sl->setSpacing(2);
    auto* scoreLbl = new QLabel("TOTAL SCORE", scoreBox);
    scoreLbl->setStyleSheet(AppTheme::summaryRowLabel());
    sl->addWidget(scoreLbl);
    m_totalScore = new QLabel("0", scoreBox);
    m_totalScore->setStyleSheet(AppTheme::scoreValue());
    sl->addWidget(m_totalScore);
    scoreBox->setLayout(sl);
    vb->addWidget(scoreBox);

    // Pause + End
    auto* bar = new QWidget(panel);
    bar->setStyleSheet(AppTheme::transparent());
    auto* bl = new QHBoxLayout(bar);
    bl->setContentsMargins(18, 12, 18, 18);
    bl->setSpacing(10);

    m_pauseBtn = new QPushButton("⏸  Pause", bar);
    m_pauseBtn->setStyleSheet(AppTheme::buttonDanger());
    m_pauseBtn->setCursor(Qt::PointingHandCursor);
    connect(m_pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_paused) { resume_system(); m_paused = false; m_pauseBtn->setText("⏸  Pause"); }
        else          { pause_system();  m_paused = true;  m_pauseBtn->setText("▶  Resume"); }
    });

    auto* endBtn = new QPushButton("End Session", bar);
    endBtn->setStyleSheet(AppTheme::buttonPrimary());
    endBtn->setCursor(Qt::PointingHandCursor);
    connect(endBtn, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);

    bl->addWidget(m_pauseBtn);
    bl->addStretch();
    bl->addWidget(endBtn);
    bar->setLayout(bl);
    vb->addWidget(bar);

    panel->setLayout(vb);
    return panel;
}

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

    SessionParameters params;
    params.sessionId      = m_sessionStart.toString("'PA_'yyyy-MM-dd_HHmm");
    params.shooterId      = "JohnD";
    params.firearm        = "Pistol, Beretta";
    params.weaponType     = m_state->categoryId();
    params.distance       = m_state->distance();
    params.shotsScheduled = qMax(10, m_shotRecords.size());
    params.drillId        = m_state->drillId();

    emit sessionEnded(SessionResult::buildMockResult(m_shotRecords, params));
}

void TrainingScreen::onTick() {
    if (m_paused) return;

    FrameResult result = process_frame(m_currentMode);

    if (!result.frame.empty()) {
        cv::Mat rgb;
        cv::cvtColor(result.frame, rgb, cv::COLOR_BGR2RGB);
        QImage img(rgb.data, rgb.cols, rgb.rows,
                   static_cast<int>(rgb.step), QImage::Format_RGB888);
        m_cameraView->setPixmap(
            QPixmap::fromImage(img.copy()).scaled(
                m_cameraView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        if (!m_camReady && m_phase == Framing) {
            m_camReady = true;
            m_guideBottom->setCurrentIndex(1);
        }
    }

    if (m_phase == Scoring && result.has_new_shot) {
        ShotRecord rec;
        rec.number    = result.shot.shot_number;
        rec.score     = result.shot.score;
        rec.splitTime = -1.0;
        // Set real image path saved by dart_system
        rec.imagePath = QString::fromStdString(get_session_folder())
                        + "/shot_" + QString::number(result.shot.shot_number) + ".png";
        m_shotRecords.append(rec);
        m_shotGrid->addShot(rec);
        m_totalScore->setText(QString::number(result.shot.total_score));
    }
}