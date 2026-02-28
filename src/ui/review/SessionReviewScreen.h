#pragma once
// src/ui/review/SessionReviewScreen.h
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include "training/data/ReviewDataTypes.h"
#include "ui/review/ShotCountTab.h"
#include "ui/review/SessionSummaryTab.h"
#include "ui/review/SessionAlbumTab.h"
#include "ui/review/ShoQReviewTab.h"

/**
 * @brief Post-session review screen with 4 tabs.
 *        Populated from SessionResult emitted by TrainingScreen.
 */
class SessionReviewScreen : public QWidget {
    Q_OBJECT
public:
    explicit SessionReviewScreen(QWidget* parent = nullptr);
    void populate(const SessionResult& result);

signals:
    void saveRequested();
    void discardRequested();

private:
    void buildUI();
    void switchTab(int index);

    QStackedWidget*    m_stack;
    QVector<QPushButton*> m_tabBtns;

    ShotCountTab*      m_shotCountTab;
    SessionSummaryTab* m_summaryTab;
    SessionAlbumTab*   m_albumTab;
    ShoQReviewTab*     m_shoqTab;
};
