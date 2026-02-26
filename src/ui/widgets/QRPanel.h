#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief QR / wristband check-in panel displayed in the sidebar.
 */
class QRPanel : public QWidget {
    Q_OBJECT
public:
    explicit QRPanel(QWidget* parent = nullptr);

private:
    QLabel*  m_titleLabel;
    QLabel*  m_hintLabel;
    QWidget* m_scanZone;
    QLabel*  m_footerLabel;
};
