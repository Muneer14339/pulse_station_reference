#include "MainWindow.h"
#include "common/AppColors.h"
#include "common/AppTheme.h"
#include <QPainter>
#include <QRadialGradient>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    using namespace AppTheme;

    setWindowTitle("PulseStation \u00B7 Lane Console");
    resize(1280, 800);
    setMinimumSize(1024, 700);

    m_state     = new SessionState(this);
    m_btManager = new BluetoothManager(this);

    auto* centralWidget = new QWidget(this);
    auto* rootLayout    = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(ContentV, ContentV, ContentV, ContentV);

    auto* consoleContainer = new QWidget(centralWidget);
    consoleContainer->setAttribute(Qt::WA_StyledBackground, true);
    consoleContainer->setStyleSheet(AppTheme::consoleContainer());
    auto* containerLayout = new QVBoxLayout(consoleContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);

    buildHeader(consoleContainer, containerLayout);

    m_stack = new QStackedWidget(consoleContainer);

    m_consoleWidget       = new ConsoleWidget(m_state, m_btManager, m_stack);
    m_reviewScreen        = new ReviewScreen(m_state, m_stack);
    m_trainingScreen = new TrainingScreen(m_state, m_btManager, m_stack);
    m_sessionReviewScreen = new SessionReviewScreen(m_stack);

    m_stack->addWidget(m_consoleWidget);        // 0
    m_stack->addWidget(m_reviewScreen);         // 1
    m_stack->addWidget(m_trainingScreen);       // 2
    m_stack->addWidget(m_sessionReviewScreen);  // 3

    containerLayout->addWidget(m_stack);
    consoleContainer->setLayout(containerLayout);
    rootLayout->addWidget(consoleContainer);
    centralWidget->setLayout(rootLayout);
    setCentralWidget(centralWidget);

    // ── Navigation ────────────────────────────────────────────────────────
    connect(m_consoleWidget, &ConsoleWidget::nextRequested,
            this, &MainWindow::showReview);

    connect(m_reviewScreen, &ReviewScreen::backRequested,
            this, &MainWindow::showConsole);
    connect(m_reviewScreen, &ReviewScreen::confirmRequested,
            this, &MainWindow::showActiveSession);

    connect(m_trainingScreen, &TrainingScreen::backRequested,
            this, &MainWindow::showReview);
    connect(m_trainingScreen, &TrainingScreen::sessionEnded,
            this, &MainWindow::showSessionReview);

    connect(m_sessionReviewScreen, &SessionReviewScreen::saveRequested,
            this, &MainWindow::showConsole);
    connect(m_sessionReviewScreen, &SessionReviewScreen::discardRequested,
            this, &MainWindow::showConsole);

    // ── Global disconnect guard ───────────────────────────────────────────
    // TrainingScreen handles its own disconnect when visible.
    // For every other screen except SessionReviewScreen → go to console.
    auto onDisconnect = [this](bool connected) {
        if (connected) return;
        QWidget* current = m_stack->currentWidget();
        if (current == m_trainingScreen)      return;
        if (current == m_sessionReviewScreen) return;
        if (current == m_consoleWidget)       return;
        showConsole();
    };
    connect(m_state, &SessionState::bluetoothConnectionChanged, this, onDisconnect);
    connect(m_state, &SessionState::cameraConnectionChanged,   this, onDisconnect);
}

void MainWindow::buildHeader(QWidget* parent, QLayout* parentLayout) {
    using namespace AppTheme;

    auto* header = new QWidget(parent);
    header->setAttribute(Qt::WA_StyledBackground, true);
    header->setStyleSheet(AppTheme::headerBar());
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(ContentH, PanelPadV, ContentH, ItemGap);
    headerLayout->setSpacing(InlineGap);

    auto* titleBlock  = new QWidget(header);
    titleBlock->setStyleSheet(AppTheme::transparent());
    auto* titleLayout = new QVBoxLayout(titleBlock);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(RowPad);
    auto* appTitle = new QLabel("PULSESTATION \u00B7 LANE CONSOLE", titleBlock);
    appTitle->setStyleSheet(AppTheme::headerAppTitle());
    auto* appSubtitle = new QLabel(
        "Wired camera to PulseAim \u00B7 Wi-Fi to cloud only \u00B7 Self-service setup.",
        titleBlock);
    appSubtitle->setStyleSheet(AppTheme::headerAppSubtitle());
    titleLayout->addWidget(appTitle);
    titleLayout->addWidget(appSubtitle);
    titleBlock->setLayout(titleLayout);

    auto* rightInfo   = new QWidget(header);
    rightInfo->setStyleSheet(AppTheme::transparent());
    auto* rightLayout = new QVBoxLayout(rightInfo);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(RowPad);
    rightLayout->setAlignment(Qt::AlignRight);
    auto* laneLabel = new QLabel(
        QString("Lane <strong style='color: %1;'>7</strong>").arg(AppTheme::rgb(AppColors::Primary())), rightInfo);
    laneLabel->setStyleSheet(AppTheme::laneLabel());
    laneLabel->setAlignment(Qt::AlignRight);
    auto* rangeLabel = new QLabel("Indoor Range \u00B7 25 yards Max", rightInfo);
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

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRadialGradient gradient(width() / 2, 0, height());
    gradient.setColorAt(0,   AppColors::Surface());
    gradient.setColorAt(0.6, AppColors::Background());
    painter.fillRect(rect(), gradient);
    QMainWindow::paintEvent(event);
}

void MainWindow::showConsole() {
    m_state->reset();
    m_stack->setCurrentWidget(m_consoleWidget);
}

void MainWindow::showReview() {
    m_reviewScreen->updateReview();
    m_stack->setCurrentWidget(m_reviewScreen);
}

void MainWindow::showActiveSession() {
    m_stack->setCurrentWidget(m_trainingScreen);
    m_trainingScreen->beginSession();
}

void MainWindow::showSessionReview(const SessionResult& result) {
    m_sessionReviewScreen->populate(result);
    m_stack->setCurrentWidget(m_sessionReviewScreen);
}