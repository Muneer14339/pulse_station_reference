#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * @brief Read-only summary card showing the current session configuration.
 */
class SummaryBox : public QWidget {
    Q_OBJECT
public:
    explicit SummaryBox(QWidget* parent = nullptr);

    void updateSummary(const QString& category,
                       const QString& caliber,
                       const QString& profile,
                       int            distance,
                       const QString& drill);

private:
    QVBoxLayout* m_layout;
};
