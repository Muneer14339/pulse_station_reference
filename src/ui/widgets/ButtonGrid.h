#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include "common/CustomButton.h"

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
    
private:
    void onButtonClicked();
    QGridLayout* m_layout;
    QMap<CustomButton*, QString> m_buttonIds;
    QString m_selectedId;
    int m_columns;
};
