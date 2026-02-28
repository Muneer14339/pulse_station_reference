#pragma once
// src/ui/MainWindow.h
#include <QMainWindow>
#include <QStackedWidget>
#include "core/SessionState.h"
#include "core/BluetoothManager.h"
#include "widgets/ConsoleWidget.h"
#include "widgets/ReviewScreen.h"
#include "training/ui/TrainingPlaceholder.h"
#include "training/ui/TrainingScreen.h"
#include "ui/review/SessionReviewScreen.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void buildHeader(QWidget* parent, QLayout* parentLayout);

    void showConsole();
    void showReview();
    void showTraining();
    void showActiveSession();
    void showSessionReview(const SessionResult& result);

    SessionState*         m_state;
    BluetoothManager*     m_btManager;
    QStackedWidget*       m_stack;

    ConsoleWidget*        m_consoleWidget;
    ReviewScreen*         m_reviewScreen;
    TrainingPlaceholder*  m_trainingPlaceholder;
    TrainingScreen*       m_trainingScreen;
    SessionReviewScreen*  m_sessionReviewScreen;   // ← new
};