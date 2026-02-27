#include "TrainingWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

TrainingWidget::TrainingWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    m_videoLabel = new QLabel(this);
    m_videoLabel->setAlignment(Qt::AlignCenter);
    m_videoLabel->setMinimumSize(600, 600);

    auto* btnRow = new QHBoxLayout();
    auto* calibBtn  = new QPushButton("Calibrate", this);
    auto* scoreBtn  = new QPushButton("Start Scoring", this);
    auto* resetBtn  = new QPushButton("Reset", this);
    auto* endBtn    = new QPushButton("End Session", this);

    btnRow->addWidget(calibBtn);
    btnRow->addWidget(scoreBtn);
    btnRow->addWidget(resetBtn);
    btnRow->addStretch();
    btnRow->addWidget(endBtn);

    layout->addWidget(m_videoLabel);
    layout->addLayout(btnRow);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &TrainingWidget::onTick);

    connect(calibBtn, &QPushButton::clicked, [this]{ m_mode = MODE_CALIBRATION; });
    connect(scoreBtn, &QPushButton::clicked, [this]{
        if (get_calibration_status().is_valid) {
            confirm_calibration();
            m_mode = MODE_SCORING;
        }
    });
    connect(resetBtn,  &QPushButton::clicked, []{ reset_session(); });
    connect(endBtn,    &QPushButton::clicked, this, &TrainingWidget::sessionEnded);
}

TrainingWidget::~TrainingWidget() { stopSession(); }

void TrainingWidget::startSession() {
    auto cameras = get_available_cameras();
    if (cameras.empty()) return;
    init_system(cameras[0].index, 600);
    
    m_timer->start(33); // ~30fps
}

void TrainingWidget::stopSession() {
    m_timer->stop();
    cleanup_system();
}

void TrainingWidget::onTick() {
    FrameResult r = process_frame(m_mode);
    if (r.frame.empty()) return;

    // Convert cv::Mat to QPixmap
    cv::Mat rgb;
    cv::cvtColor(r.frame, rgb, cv::COLOR_BGR2RGB);
    QImage img(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    m_videoLabel->setPixmap(QPixmap::fromImage(img.copy()));
}