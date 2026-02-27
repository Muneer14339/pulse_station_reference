#pragma once
#include <QWidget>
#include <QTimer>
#include <QLabel>
#include "../engine/dart_system.h"

class TrainingWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrainingWidget(QWidget* parent = nullptr);
    ~TrainingWidget();
    void startSession();
    void stopSession();

signals:
    void sessionEnded();

private slots:
    void onTick();

private:
    QTimer*  m_timer;
    QLabel*  m_videoLabel;
    int      m_mode = MODE_CALIBRATION;
};