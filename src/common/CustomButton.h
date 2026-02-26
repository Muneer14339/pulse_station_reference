#pragma once
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief Selectable grid button with a main label and optional sub-label.
 *
 * Used in ButtonGrid to represent choices (category, caliber, profile, etc.).
 * Visual state (normal / hover / selected) is driven by AppTheme.
 */
class CustomButton : public QPushButton {
    Q_OBJECT
public:
    explicit CustomButton(const QString& mainText,
                          const QString& subText = QString(),
                          QWidget* parent = nullptr);

    void setMainText(const QString& text);
    void setSubText(const QString& text);
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void applyStyle();

    QLabel* m_mainLabel;
    QLabel* m_subLabel;
    bool    m_selected = false;
};
