#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "core/SessionState.h"
#include "core/DataModels.h"
#include "SummaryBox.h"

/**
 * @brief Review screen (Screen 2) — shows session summary before confirming.
 */
class ReviewScreen : public QWidget {
    Q_OBJECT
public:
    explicit ReviewScreen(SessionState* state, QWidget* parent = nullptr);

    /** Call before showing this screen to refresh the summary box. */
    void updateReview();

signals:
    void backRequested();
    void confirmRequested();

private:
    SessionState*            m_state;
    QMap<QString, Category>  m_categories;
    SummaryBox*              m_summaryBox;
    QPushButton*             m_backBtn;
    QPushButton*             m_confirmBtn;
};
