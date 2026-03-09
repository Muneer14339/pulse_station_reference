#include "TrainingScreen.h"
#include "training/data/ScoringGuideData.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"
#include <QHBoxLayout>

TrainingScreen::TrainingScreen(SessionState* state, BluetoothManager* btManager, QWidget* parent)
    : QWidget(parent), m_state(state), m_btManager(btManager)
{
    buildUI();

    auto onDisconnect = [this](bool connected) {
        if (connected || !isVisible()) return;
        stopAndExit();
    };
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
    m_shotLimit = 0;
    for (const Drill& d : DataModels::getDrills())
      if (d.id == m_state->drillId()) { m_shotLimit = d.shotLimit; break; }
    m_sessionStart = QDateTime::currentDateTime();
    m_shotGrid->clear();
    m_totalScore->setText("0");
    m_rightStack->setCurrentIndex(0);
    m_guideBottom->setCurrentIndex(0);
    m_step4->hide();

    // Fresh correlator each session — auto-disconnects previous via deleteLater
    if (m_correlator) m_correlator->deleteLater();
    m_correlator = new ShotCorrelator(this);
    connect(m_btManager,   &BluetoothManager::shotSignalReceived,
            m_correlator,  &ShotCorrelator::onBLEShot);
    connect(m_correlator,  &ShotCorrelator::shotFinalized,
            this,          &TrainingScreen::onShotFinalized);
    connect(m_correlator, &ShotCorrelator::shotPending,
        m_shotGrid,   &ShotGridWidget::setPendingShot);

    init_system(m_state->cameraIndex(), 600);
    m_tickTimer->start();
}


static QWidget* makeStatusBar(QWidget* parent) {
    using namespace AppTheme;
    auto* bar = new QWidget(parent);
    bar->setStyleSheet(AppTheme::transparent());
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, ItemGap);
    l->setSpacing(InlineGap);
    l->addStretch();
    for (const char* txt : {"\u25CF Wi-Fi", "\u2B21  BLE"}) {
        auto* lbl = new QLabel(txt, bar);
        lbl->setStyleSheet(AppTheme::connectedIcon());
        l->addWidget(lbl);
        if (txt == std::string("\u25CF Wi-Fi")) l->addSpacing(ItemGap);
    }
    bar->setLayout(l);
    return bar;
}

static QWidget* makeTopBarWithBack(QWidget* parent, QObject* ctx,
                                    const std::function<void()>& backAction)
{
    using namespace AppTheme;
    auto* bar = new QWidget(parent);
    bar->setStyleSheet(AppTheme::transparent());
    auto* l = new QHBoxLayout(bar);
    l->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, ItemGap);
    l->setSpacing(InlineGap);
    auto* btn = new QPushButton("\u2190", bar);
    btn->setStyleSheet(AppTheme::iconButton());
    btn->setFixedSize(IconBtnSz, IconBtnSz);
    btn->setCursor(Qt::PointingHandCursor);
    QObject::connect(btn, &QPushButton::clicked, ctx, [backAction]{ backAction(); });
    l->addWidget(btn);
    l->addStretch();
    for (const char* txt : {"\u25CF Wi-Fi", "\u2B21  BLE"}) {
        auto* lbl = new QLabel(txt, bar);
        lbl->setStyleSheet(AppTheme::connectedIcon());
        l->addWidget(lbl);
        if (txt == std::string("\u25CF Wi-Fi")) l->addSpacing(ItemGap);
    }
    bar->setLayout(l);
    return bar;
}

static QWidget* makeStep(QWidget* parent, const GuideStep& step) {
    using namespace AppTheme;
    auto* block = new QWidget(parent);
    block->setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(block);
    vb->setContentsMargins(0, 0, 0, ItemGap);
    vb->setSpacing(InlineGap);
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
    using namespace AppTheme;

    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    vb->addWidget(makeTopBarWithBack(panel, this, [this]{ cancelSession(); }));

    auto* scroll = new QScrollArea(panel);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::transparent());
    auto* content = new QWidget;
    content->setStyleSheet(AppTheme::transparent());
    auto* cl = new QVBoxLayout(content);
    cl->setContentsMargins(PanelPadH, PanelPadV, PanelPadH, PanelPadV);
    cl->setSpacing(SectionGap);

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

    auto makeBottomBar = [&](QWidget* w) {
        auto* pg = new QWidget;
        auto* l  = new QHBoxLayout(pg);
        l->setContentsMargins(PanelPadH, ItemGap, PanelPadH, PanelPadV);
        l->setSpacing(InlineGap);
        l->addStretch();
        l->addWidget(w);
        pg->setLayout(l);
        return pg;
    };

    { auto* b = new QPushButton("Loading...");
      b->setStyleSheet(AppTheme::buttonPrimary()); b->setEnabled(false);
      m_guideBottom->addWidget(makeBottomBar(b)); }

    { auto* b = new QPushButton("Frame Target");
      b->setStyleSheet(AppTheme::buttonPrimary());
      b->setCursor(Qt::PointingHandCursor);
      connect(b, &QPushButton::clicked, this, &TrainingScreen::enterCalibrating);
      m_guideBottom->addWidget(makeBottomBar(b)); }

    { auto* pg = new QWidget;
      auto* l  = new QHBoxLayout(pg);
      l->setContentsMargins(PanelPadH, ItemGap, PanelPadH, PanelPadV);
      l->setSpacing(InlineGap);
      l->addStretch();
      auto* stop = new QPushButton("Stop", pg);
      stop->setStyleSheet(AppTheme::buttonDanger());
      stop->setCursor(Qt::PointingHandCursor);
      connect(stop, &QPushButton::clicked, this, &TrainingScreen::stopAndExit);
      auto* sc = new QPushButton("Start Scoring", pg);
      sc->setStyleSheet(AppTheme::buttonPrimary());
      sc->setCursor(Qt::PointingHandCursor);
      connect(sc, &QPushButton::clicked, this, &TrainingScreen::enterScoring);
      l->addWidget(stop);
      l->addWidget(sc);
      pg->setLayout(l);
      m_guideBottom->addWidget(pg); }

    vb->addWidget(m_guideBottom);
    panel->setLayout(vb);
    return panel;
}

QWidget* TrainingScreen::buildScoringPanel() {
    using namespace AppTheme;

    auto* panel = new QWidget(this);
    panel->setStyleSheet(AppTheme::sidePanel());
    auto* vb = new QVBoxLayout(panel);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->setSpacing(0);

    vb->addWidget(makeStatusBar(panel));

    auto* gridWrapper = new QWidget(panel);
    gridWrapper->setStyleSheet(AppTheme::transparent());
    auto* gw = new QVBoxLayout(gridWrapper);
    gw->setContentsMargins(PanelPadH, 0, PanelPadH, 0);
    gw->setSpacing(0);
    m_shotGrid = new ShotGridWidget(gridWrapper);
    gw->addWidget(m_shotGrid);
    gridWrapper->setLayout(gw);
    vb->addWidget(gridWrapper, 1);

    auto* scoreWrapper = new QWidget(panel);
    scoreWrapper->setStyleSheet(AppTheme::transparent());
    auto* sw = new QVBoxLayout(scoreWrapper);
    sw->setContentsMargins(PanelPadH, ItemGap, PanelPadH, 0);
    sw->setSpacing(0);
    auto* scoreBox = new QWidget(scoreWrapper);
    scoreBox->setAttribute(Qt::WA_StyledBackground, true);
    scoreBox->setStyleSheet(AppTheme::scorePanel());
    auto* sl = new QVBoxLayout(scoreBox);
    sl->setContentsMargins(CardPadH, CardPadV, CardPadH, CardPadV);
    sl->setSpacing(RowPad);
    auto* scoreLbl = new QLabel("TOTAL SCORE", scoreBox);
    scoreLbl->setStyleSheet(AppTheme::summaryRowLabel());
    sl->addWidget(scoreLbl);
    m_totalScore = new QLabel("0", scoreBox);
    m_totalScore->setStyleSheet(AppTheme::scoreValue());
    sl->addWidget(m_totalScore);
    scoreBox->setLayout(sl);
    sw->addWidget(scoreBox);
    scoreWrapper->setLayout(sw);
    vb->addWidget(scoreWrapper);

    auto* bar = new QWidget(panel);
    bar->setStyleSheet(AppTheme::transparent());
    auto* bl = new QHBoxLayout(bar);
    bl->setContentsMargins(PanelPadH, ItemGap, PanelPadH, PanelPadV);
    bl->setSpacing(InlineGap);

    m_pauseBtn = new QPushButton("\u23F8  Pause", bar);
    m_pauseBtn->setStyleSheet(AppTheme::buttonDanger());
    m_pauseBtn->setCursor(Qt::PointingHandCursor);
    connect(m_pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_paused) { resume_system(); m_paused = false; m_pauseBtn->setText("\u23F8  Pause"); }
        else          { pause_system();  m_paused = true;  m_pauseBtn->setText("\u25B6  Resume"); }
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

void TrainingScreen::cancelSession() {
    m_tickTimer->stop();
    if (m_correlator) m_correlator->reset();
    cleanup_system();
    emit backRequested();
}

void TrainingScreen::stopAndExit() {
    m_tickTimer->stop();
    if (!m_correlator) { doEndSession(); return; }
    m_drainConn = connect(m_correlator, &ShotCorrelator::drained,
                          this, &TrainingScreen::doEndSession);
    m_correlator->beginDrain();
}

void TrainingScreen::doEndSession() {
    disconnect(m_drainConn);
    if (m_correlator) m_correlator->reset();
    cleanup_system();

    SessionParameters params;
    params.sessionId      = m_sessionStart.toString("'PA_'yyyy-MM-dd_HHmm");
    params.shooterId      = "JohnD";
    params.firearm        = "Pistol, Beretta";
    params.weaponType     = m_state->categoryId();
    params.distance       = m_state->distance();
    params.shotsScheduled = m_shotLimit;
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
        const QString imgPath = QString::fromStdString(get_session_folder())
                               + "/shot_" + QString::number(result.shot.shot_number) + ".png";
        m_correlator->onCameraShot(result.shot.score, result.shot.direction, imgPath);
    }
}


// ── New slot — handles finalized shots from correlator ────────────────────────
void TrainingScreen::onShotFinalized(const ShotRecord& rec) {
    m_shotRecords.append(rec);
    m_shotGrid->finalizePendingRow(rec);
    int total = 0;
    for (const auto& r : m_shotRecords) total += r.score;
    m_totalScore->setText(QString::number(total));

    if (m_shotLimit > 0 && m_shotRecords.size() >= m_shotLimit)
        stopAndExit();   // ← auto-end; user can still end early via button
}