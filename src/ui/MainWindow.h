#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "core/SessionState.h"
#include "widgets/ConsoleWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    void showFinalScreen();
    void showMainScreen();
    
    SessionState* m_state;
    QStackedWidget* m_stack;
    ConsoleWidget* m_consoleWidget;
    QWidget* m_finalScreen;
};
