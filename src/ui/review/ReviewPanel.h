#pragma once
// src/ui/review/ReviewPanel.h
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * @brief Reusable info panel: title → divider → key/value rows.
 *        Used in ShotCountTab, SessionSummaryTab.
 *        Styles mapped entirely to existing AppTheme.h methods.
 */
class ReviewPanel : public QWidget {
    Q_OBJECT
public:
    explicit ReviewPanel(const QString& icon,
                         const QString& title,
                         QWidget*       parent = nullptr);

    void addRow(const QString& key, const QString& value, bool highlight = false);
    void clearRows();

private:
    QVBoxLayout* m_rowsLayout;
};
