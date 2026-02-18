#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "core/SessionState.h"
#include "core/BluetoothManager.h"
#include "widgets/ConsoleWidget.h"
#include "widgets/ReviewScreen.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    void showReviewScreen();
    void showFinalScreen();
    void showMainScreen();
    
    SessionState* m_state;
    BluetoothManager* m_btManager;
    QStackedWidget* m_stack;
    ConsoleWidget* m_consoleWidget;
    ReviewScreen* m_reviewScreen;
    QWidget* m_finalScreen;
};