#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QMap>
#include "common/CustomButton.h"

/**
 * @brief Responsive grid of CustomButtons for option selection.
 *
 * Manages single-selection state automatically.
 */
class ButtonGrid : public QWidget {
    Q_OBJECT
public:
    explicit ButtonGrid(int columns = 3, QWidget* parent = nullptr);

    void addButton(const QString& mainText, const QString& subText, const QString& id);
    void clear();
    void setSelectedId(const QString& id);
    QString selectedId() const { return m_selectedId; }

signals:
    void buttonClicked(const QString& id);

private slots:
    void onButtonClicked();

private:
    QGridLayout*             m_layout;
    QMap<CustomButton*, QString> m_buttonIds;
    QString                  m_selectedId;
    int                      m_columns;
};
