#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

/**
 * @brief Transient notification banner anchored to the top-right of the parent.
 *
 * Usage:  SnackBar::show(parentWidget, "Message", SnackBar::Success);
 */
class SnackBar : public QWidget {
    Q_OBJECT
public:
    enum Type { Info, Success, Error };

    static void show(QWidget* parent,
                     const QString& message,
                     Type type     = Info,
                     int  duration = 3000);

private:
    explicit SnackBar(QWidget* parent, const QString& message, Type type, int duration);
    void buildUI();
    void animateIn();

    QString m_message;
    Type    m_type;
    int     m_duration;
    QLabel* m_label;
    QTimer* m_timer;
};
