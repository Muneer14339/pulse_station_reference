#pragma once
// src/ui/widgets/ShotGridWidget.h
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "training/data/ReviewDataTypes.h"

/**
 * @brief 4-column shot data table.
 *        Full-height column dividers via overlay QWidgets (resized in resizeEvent).
 *        Zero hardcoded colors — all styles from AppTheme.
 */
class ShotGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShotGridWidget(QWidget* parent = nullptr);

    void addShot(const ShotRecord& s);
    void populate(const QVector<ShotRecord>& shots);
    void setPendingShot(int shotNumber);
void finalizePendingRow(const ShotRecord& s);
    void clear();

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    QWidget*     buildRow(const ShotRecord& s);
    void         repositionDividers();
    QWidget* buildPendingRow(int shotNumber);

    QWidget*         m_header     = nullptr;
    QScrollArea*     m_scroll     = nullptr;
    QVBoxLayout*     m_bodyLayout = nullptr;
    QList<QWidget*>  m_colDividers;   // 3 overlay lines for 4 columns
    QWidget* m_pendingRow = nullptr;
};