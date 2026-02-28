#pragma once
#include <QString>
#include <QVector>

/**
 * @brief Static content for the Realtime Scoring Guide.
 *        Kept separate from UI so text changes don't touch widget code.
 */
struct GuideStep {
    QString title;
    QVector<QString> bullets;
};

inline QVector<GuideStep> scoringGuideSteps() {
    return {
        {
            "1. Start",
            {
                "• Attach AimSync BLE device to your finger.",
                "• Press Start to begin detection, press again to start scoring.",
                "• After the start beep, fire the first shot."
            }
        },
        {
            "2. During Shooting",
            {
                "• Beep = shot detected → fire the next shot.",
                "• No beep = shot missed (score 0).",
                "• Always wait for the beep before firing again."
            }
        },
        {
            "3. Pause / Resume",
            {
                "• Tap Pause anytime.",
                "• Tap Resume to continue."
            }
        }
    };
}

inline GuideStep scoringGuideStep4() {
    return {
        "4. Finish",
        { "• Session automatically finishes after all shots." }
    };
}
