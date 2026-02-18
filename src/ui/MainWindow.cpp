#include "MainWindow.h"
#include "common/AppColors.h"
#include "common/AppTheme.h"
#include "widgets/SummaryBox.h"
#include <QPainter>
#include <QRadialGradient>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("PulseStation · Lane Console");
    resize(1280, 800);
    
    m_state = new SessionState(this);
    m_btManager = new BluetoothManager(this);
    
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    auto* consoleContainer = new QWidget(centralWidget);
    consoleContainer->setStyleSheet(
        "background: rgb(17, 23, 41); "
        "border: 1px solid rgba(255, 255, 255, 20); "
        "border-radius: 22px;"
    );
    
    auto* containerLayout = new QVBoxLayout(consoleContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    
    auto* header = new QWidget(consoleContainer);
    header->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop: 0 rgba(9, 13, 30, 242), stop: 1 rgba(13, 19, 40, 242)); "
        "border: none; border-bottom: 1px solid rgba(255, 255, 255, 15); }"
    );
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(24, 14, 24, 10);
    
    auto* titleBlock = new QWidget(header);
    titleBlock->setStyleSheet("background: transparent; border: none;");
    auto* titleLayout = new QVBoxLayout(titleBlock);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(4);
    
    auto* title = new QLabel("PULSESTATION · LANE CONSOLE", titleBlock);
    title->setStyleSheet("font-size: 20px; letter-spacing: 2px; font-weight: bold; color: rgb(245, 245, 255); background: transparent; border: none;");
    
    auto* subtitle = new QLabel("Wired camera to PulseAim. Wi-Fi to cloud only. Self-service setup.", titleBlock);
    subtitle->setStyleSheet("font-size: 12px; color: rgb(179, 185, 214); background: transparent; border: none;");
    
    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);
    titleBlock->setLayout(titleLayout);
    
    auto* rightInfo = new QWidget(header);
    rightInfo->setStyleSheet("background: transparent; border: none;");
    auto* rightLayout = new QVBoxLayout(rightInfo);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(2);
    rightLayout->setAlignment(Qt::AlignRight);
    
    auto* laneLabel = new QLabel("Lane <strong style='color: rgb(255, 182, 73);'>7</strong>", rightInfo);
    laneLabel->setStyleSheet("font-size: 14px; color: rgb(199, 204, 240); background: transparent; border: none;");
    laneLabel->setAlignment(Qt::AlignRight);
    
    auto* rangeLabel = new QLabel("Indoor Range · 25 yards Max", rightInfo);
    rangeLabel->setStyleSheet("font-size: 12px; color: rgb(179, 185, 214); background: transparent; border: none;");
    rangeLabel->setAlignment(Qt::AlignRight);
    
    rightLayout->addWidget(laneLabel);
    rightLayout->addWidget(rangeLabel);
    rightInfo->setLayout(rightLayout);
    
    headerLayout->addWidget(titleBlock);
    headerLayout->addStretch();
    headerLayout->addWidget(rightInfo);
    header->setLayout(headerLayout);
    
    m_stack = new QStackedWidget(consoleContainer);
    m_consoleWidget = new ConsoleWidget(m_state, m_btManager, m_stack);
    m_stack->addWidget(m_consoleWidget);
    
    m_reviewScreen = new ReviewScreen(m_state, m_stack);
    m_stack->addWidget(m_reviewScreen);
    
    m_finalScreen = new QWidget(m_stack);
    m_finalScreen->setStyleSheet("background: transparent; border: none;");
    auto* finalLayout = new QVBoxLayout(m_finalScreen);
    finalLayout->setContentsMargins(40, 40, 40, 40);
    finalLayout->setSpacing(20);
    
    auto* finalTitle = new QLabel("Session Ready", m_finalScreen);
    finalTitle->setStyleSheet("font-size: 28px; font-weight: 600; color: rgb(230, 233, 255); background: transparent; border: none;");
    finalTitle->setAlignment(Qt::AlignCenter);
    
    auto* finalSubtitle = new QLabel("Your lane is configured and ready to begin training.", m_finalScreen);
    finalSubtitle->setStyleSheet("font-size: 16px; color: rgb(179, 185, 214); background: transparent; border: none;");
    finalSubtitle->setAlignment(Qt::AlignCenter);
    
    auto* finalSummary = new SummaryBox(m_finalScreen);
    
    auto* newSessionBtn = new QPushButton("Start New Session", m_finalScreen);
    newSessionBtn->setStyleSheet(AppTheme::getButtonPrimaryStyle());
    newSessionBtn->setCursor(Qt::PointingHandCursor);
    newSessionBtn->setFixedWidth(300);
    
    finalLayout->addStretch();
    finalLayout->addWidget(finalTitle);
    finalLayout->addWidget(finalSubtitle);
    finalLayout->addSpacing(20);
    finalLayout->addWidget(finalSummary);
    finalLayout->addSpacing(20);
    finalLayout->addWidget(newSessionBtn, 0, Qt::AlignCenter);
    finalLayout->addStretch();
    m_finalScreen->setLayout(finalLayout);
    
    m_stack->addWidget(m_finalScreen);
    
    containerLayout->addWidget(header);
    containerLayout->addWidget(m_stack);
    consoleContainer->setLayout(containerLayout);
    
    mainLayout->addWidget(consoleContainer);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    
    connect(m_consoleWidget, &ConsoleWidget::nextRequested, this, &MainWindow::showReviewScreen);
    connect(m_reviewScreen, &ReviewScreen::backRequested, this, &MainWindow::showMainScreen);
    connect(m_reviewScreen, &ReviewScreen::confirmRequested, this, &MainWindow::showFinalScreen);
    connect(newSessionBtn, &QPushButton::clicked, this, &MainWindow::showMainScreen);
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRadialGradient gradient(width() / 2, 0, height());
    gradient.setColorAt(0, AppColors::BackgroundGradientTop);
    gradient.setColorAt(0.6, AppColors::BackgroundGradientBottom);
    
    painter.fillRect(rect(), gradient);
    
    QMainWindow::paintEvent(event);
}

void MainWindow::showReviewScreen() {
    m_reviewScreen->updateReview();
    m_stack->setCurrentWidget(m_reviewScreen);
}

void MainWindow::showFinalScreen() {
    m_stack->setCurrentWidget(m_finalScreen);
}

void MainWindow::showMainScreen() {
    m_state->reset();
    m_stack->setCurrentWidget(m_consoleWidget);
}