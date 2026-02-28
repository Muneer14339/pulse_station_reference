#pragma once
// src/ui/review/ShoQReviewTab.h
#include <QWidget>
#include "training/data/ReviewDataTypes.h"

class ShoQReviewTab : public QWidget {
    Q_OBJECT
public:
    explicit ShoQReviewTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);
private:
    QWidget* m_content = nullptr;
};
