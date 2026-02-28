#pragma once
// src/ui/review/ShotCountTab.h
#include <QWidget>
#include "training/data/ReviewDataTypes.h"
#include "ui/widgets/ShotGridWidget.h"          // ← shared widget
#include "ui/review/ReviewPanel.h"

class ShotCountTab : public QWidget {
    Q_OBJECT
public:
    explicit ShotCountTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);

private:
    ShotGridWidget* m_shotGrid;
    ReviewPanel*    m_paramsPanel;
    ReviewPanel*    m_statsPanel;
    ReviewPanel*    m_metricsPanel;
};
