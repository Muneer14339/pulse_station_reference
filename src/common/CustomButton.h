#pragma once
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class CustomButton : public QPushButton {
    Q_OBJECT
public:
    explicit CustomButton(const QString& mainText, const QString& subText = "", QWidget* parent = nullptr);
    void setMainText(const QString& text);
    void setSubText(const QString& text);
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }
    
protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    
private:
    void updateStyle();
    QLabel* m_mainLabel;
    QLabel* m_subLabel;
    bool m_selected;
};