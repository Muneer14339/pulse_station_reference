// src/ui/MainWindow.cpp
#include "MainWindow.h"
#include "common/AppColors.h"
#include "common/AppTheme.h"
#include <QPainter>
#include <QRadialGradient>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("PulseStation · Lane Console");
    resize(1280, 800);

    m_state     = new SessionState(this);
    m_btManager = new BluetoothManager(this);

    // ── Root layout ───────────────────────────────────────────────────────
    auto* centralWidget = new QWidget(this);
    auto* rootLayout    = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(20, 20, 20, 20);

    auto* consoleContainer = new QWidget(centralWidget);
    consoleContainer->setStyleSheet(AppTheme::consoleContainer());
    auto* containerLayout = new QVBoxLayout(consoleContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    buildHeader(consoleContainer, containerLayout);

    // ── Screen stack ──────────────────────────────────────────────────────
    m_stack = new QStackedWidget(consoleContainer);

    m_consoleWidget       = new ConsoleWidget(m_state, m_btManager, m_stack);
    m_reviewScreen        = new ReviewScreen(m_state, m_stack);
    m_trainingPlaceholder = new TrainingPlaceholder(m_state, m_stack);
    m_trainingScreen      = new TrainingScreen(m_state, m_stack);
    m_sessionReviewScreen = new SessionReviewScreen(m_stack);   // ← new

    m_stack->addWidget(m_consoleWidget);        // 0
    m_stack->addWidget(m_reviewScreen);         // 1
    m_stack->addWidget(m_trainingPlaceholder);  // 2
    m_stack->addWidget(m_trainingScreen);       // 3
    m_stack->addWidget(m_sessionReviewScreen);  // 4

    containerLayout->addWidget(m_stack);
    consoleContainer->setLayout(containerLayout);
    rootLayout->addWidget(consoleContainer);
    centralWidget->setLayout(rootLayout);
    setCentralWidget(centralWidget);

    // ── Navigation wiring ─────────────────────────────────────────────────
    connect(m_consoleWidget, &ConsoleWidget::nextRequested,
            this, &MainWindow::showReview);

    connect(m_reviewScreen, &ReviewScreen::backRequested,
            this, &MainWindow::showConsole);
    connect(m_reviewScreen, &ReviewScreen::confirmRequested,
            this, &MainWindow::showTraining);

    connect(m_trainingPlaceholder, &TrainingPlaceholder::newSessionRequested,
            this, &MainWindow::showConsole);
    connect(m_trainingPlaceholder, &TrainingPlaceholder::sessionStarted,
            this, &MainWindow::showActiveSession);

    // TrainingScreen now emits SessionResult → go to review screen
    connect(m_trainingScreen, &TrainingScreen::sessionEnded,
            this, &MainWindow::showSessionReview);

    // Save / Discard from review → back to training placeholder
    connect(m_sessionReviewScreen, &SessionReviewScreen::saveRequested,
            this, &MainWindow::showTraining);
    connect(m_sessionReviewScreen, &SessionReviewScreen::discardRequested,
            this, &MainWindow::showTraining);
}

// ── Header ────────────────────────────────────────────────────────────────────

void MainWindow::buildHeader(QWidget* parent, QLayout* parentLayout) {
    auto* header = new QWidget(parent);
    header->setStyleSheet(AppTheme::headerBar());
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(26, 16, 26, 12);

    auto* titleBlock  = new QWidget(header);
    titleBlock->setStyleSheet(AppTheme::transparent());
    auto* titleLayout = new QVBoxLayout(titleBlock);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(4);

    auto* appTitle = new QLabel("PULSESTATION · LANE CONSOLE", titleBlock);
    appTitle->setStyleSheet(AppTheme::headerAppTitle());
    auto* appSubtitle = new QLabel(
        "Wired camera to PulseAim · Wi-Fi to cloud only · Self-service setup.", titleBlock);
    appSubtitle->setStyleSheet(AppTheme::headerAppSubtitle());
    titleLayout->addWidget(appTitle);
    titleLayout->addWidget(appSubtitle);
    titleBlock->setLayout(titleLayout);

    auto* rightInfo   = new QWidget(header);
    rightInfo->setStyleSheet(AppTheme::transparent());
    auto* rightLayout = new QVBoxLayout(rightInfo);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(3);
    rightLayout->setAlignment(Qt::AlignRight);

    auto* laneLabel = new QLabel(
        "Lane <strong style='color: rgb(255,182,73);'>7</strong>", rightInfo);
    laneLabel->setStyleSheet(AppTheme::laneLabel());
    laneLabel->setAlignment(Qt::AlignRight);
    auto* rangeLabel = new QLabel("Indoor Range · 25 yards Max", rightInfo);
    rangeLabel->setStyleSheet(AppTheme::laneSublabel());
    rangeLabel->setAlignment(Qt::AlignRight);
    rightLayout->addWidget(laneLabel);
    rightLayout->addWidget(rangeLabel);
    rightInfo->setLayout(rightLayout);

    headerLayout->addWidget(titleBlock);
    headerLayout->addStretch();
    headerLayout->addWidget(rightInfo);
    header->setLayout(headerLayout);
    parentLayout->addWidget(header);
}

// ── Paint ─────────────────────────────────────────────────────────────────────

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRadialGradient gradient(width() / 2, 0, height());
    gradient.setColorAt(0,   AppColors::BackgroundGradientTop);
    gradient.setColorAt(0.6, AppColors::BackgroundGradientBottom);
    painter.fillRect(rect(), gradient);
    QMainWindow::paintEvent(event);
}

// ── Navigation ────────────────────────────────────────────────────────────────

void MainWindow::showConsole() {
    m_state->reset();
    m_stack->setCurrentWidget(m_consoleWidget);
}

void MainWindow::showReview() {
    m_reviewScreen->updateReview();
    m_stack->setCurrentWidget(m_reviewScreen);
}

void MainWindow::showTraining() {
    m_trainingPlaceholder->refresh();
    m_stack->setCurrentWidget(m_trainingPlaceholder);
}

void MainWindow::showActiveSession() {
    m_stack->setCurrentWidget(m_trainingScreen);
    m_trainingScreen->beginSession();
}

void MainWindow::showSessionReview(const SessionResult& result) {
    m_sessionReviewScreen->populate(result);
    m_stack->setCurrentWidget(m_sessionReviewScreen);
}