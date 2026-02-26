#pragma once
#include <QWidget>
#include <QPushButton>
#include "core/SessionState.h"
#include "SummaryBox.h"

/**
 * @brief Placeholder for the upcoming Training Session screen (Screen 3).
 *
 * This screen is shown after the user confirms their session configuration.
 * The camera streaming and shot-processing feature will be integrated here
 * in a future sprint — for now it displays session details and a stub
 * "camera feed" area so the overall UI flow is complete and testable.
 *
 * To integrate the training feature:
 *   1. Replace m_cameraPlaceholder with your actual camera widget.
 *   2. Connect BluetoothManager data signals to your processing pipeline.
 *   3. Remove the "Coming Soon" label.
 */
class TrainingPlaceholder : public QWidget {
    Q_OBJECT
public:
    explicit TrainingPlaceholder(SessionState* state, QWidget* parent = nullptr);

    /** Refresh the summary panel when the screen becomes visible. */
    void refresh();

signals:
    void newSessionRequested();

private:
    void buildUI();

    SessionState* m_state;
    SummaryBox*   m_summaryBox;
    QPushButton*  m_newSessionBtn;
    QWidget*      m_cameraPlaceholder; // TODO: replace with real camera widget
};
