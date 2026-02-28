#pragma once
// src/ui/review/SessionSummaryTab.h
#include <QWidget>
#include "training/data/ReviewDataTypes.h"
#include "ui/review/ReviewPanel.h"

class SessionSummaryTab : public QWidget {
    Q_OBJECT
public:
    explicit SessionSummaryTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);
private:
    ReviewPanel* m_performancePanel;
    ReviewPanel* m_paramsPanel;
};
