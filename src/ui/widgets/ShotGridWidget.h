#pragma once
// src/ui/widgets/ShotGridWidget.h
// ── Shared shot table widget — used in TrainingScreen (live) and ShotCountTab (review) ─
#include <QWidget>
#include <QVBoxLayout>
#include "training/data/ReviewDataTypes.h"

class ShotGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShotGridWidget(QWidget* parent = nullptr);

    /** Populate all at once (review screen). */
    void populate(const QVector<ShotRecord>& shots);

    /** Append one row live (training screen). */
    void addShot(const ShotRecord& s);

    /** Clear all rows. */
    void clear();

private:
    QWidget* buildRow(const ShotRecord& s);
    QVBoxLayout* m_bodyLayout;
};
