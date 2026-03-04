#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "core/SessionState.h"
#include "core/DataModels.h"
#include "SummaryBox.h"
#include "training/ui/CameraPanel.h"

class ReviewScreen : public QWidget {
    Q_OBJECT
public:
    explicit ReviewScreen(SessionState* state, QWidget* parent = nullptr);
    void updateReview();

signals:
    void backRequested();
    void confirmRequested();

private:
    void buildUI();
    void updateConfirmState();

    SessionState*           m_state;
    QMap<QString, Category> m_categories;

    CameraPanel* m_cameraPanel;
    SummaryBox*  m_summaryBox;
    QPushButton* m_backBtn;
    QPushButton* m_confirmBtn;
};