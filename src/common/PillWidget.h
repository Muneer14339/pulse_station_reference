#pragma once
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class PillWidget : public QWidget {
    Q_OBJECT
public:
    explicit PillWidget(const QString& text, QWidget* parent = nullptr);
    void setActive(bool active);
    bool isActive() const { return m_active; }
    
private:
    void updateStyle();
    QWidget* m_dot;
    QLabel* m_label;
    bool m_active;
};
