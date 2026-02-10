#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class QRPanel : public QWidget {
    Q_OBJECT
public:
    explicit QRPanel(QWidget* parent = nullptr);
    
private:
    QLabel* m_titleLabel;
    QLabel* m_hintLabel;
    QWidget* m_qrBox;
    QLabel* m_footerLabel;
};
