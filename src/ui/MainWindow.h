#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "core/SessionState.h"
#include "core/BluetoothManager.h"
#include "widgets/ConsoleWidget.h"
#include "widgets/ReviewScreen.h"
#include "training/ui/TrainingPlaceholder.h" 

/**
 * @brief Application shell — owns the header bar and the 3-screen stack.
 *
 * Screen flow:
 *   ConsoleWidget  →  ReviewScreen  →  TrainingPlaceholder
 *        ↑                  ↑
 *    (new session)      (back btn)
 */
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

    SessionState*         m_state;
    BluetoothManager*     m_btManager;
    QStackedWidget*       m_stack;
    ConsoleWidget*        m_consoleWidget;
    ReviewScreen*         m_reviewScreen;
    TrainingPlaceholder*  m_trainingScreen;
};
