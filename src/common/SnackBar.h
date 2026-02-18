#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

class SnackBar : public QWidget {
    Q_OBJECT
public:
    enum Type { Info, Success, Error };
    
    static void show(QWidget* parent, const QString& message, Type type = Info, int duration = 3000);
    
private:
    explicit SnackBar(QWidget* parent, const QString& message, Type type, int duration);
    void setupUI();
    void showAnimation();
    
    QString m_message;
    Type m_type;
    int m_duration;
    QLabel* m_label;
    QTimer* m_timer;
};