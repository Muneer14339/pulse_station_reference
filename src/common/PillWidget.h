#pragma once
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

/**
 * @brief Small pill-shaped status indicator used in StepFlow.
 *
 * Active pills use the orange accent; inactive ones are dim.
 */
class PillWidget : public QWidget {
    Q_OBJECT
public:
    explicit PillWidget(const QString& text, QWidget* parent = nullptr);
    void setActive(bool active);
    bool isActive() const { return m_active; }

private:
    void applyStyle();

    QWidget* m_dot;
    QLabel*  m_label;
    bool     m_active = false;
};
