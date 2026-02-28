// src/training/data/ReviewDataTypes.cpp
#include "ReviewDataTypes.h"
#include <algorithm>

int SessionResult::totalScore() const {
    int sum = 0;
    for (const auto& s : shots) sum += s.score;
    return sum;
}

double SessionResult::avgScore() const {
    if (shots.isEmpty()) return 0.0;
    return static_cast<double>(totalScore()) / shots.size();
}

double SessionResult::bestSplit() const {
    double best = 9999.0;
    for (const auto& s : shots)
        if (s.splitTime > 0 && s.splitTime < best) best = s.splitTime;
    return (best < 9999.0) ? best : 0.0;
}

double SessionResult::avgSplit() const {
    double sum = 0.0; int cnt = 0;
    for (const auto& s : shots)
        if (s.splitTime > 0) { sum += s.splitTime; ++cnt; }
    return cnt ? sum / cnt : 0.0;
}

SessionResult SessionResult::buildMockResult(const QVector<ShotRecord>& shots,
                                              const SessionParameters&   params)
{
    SessionResult r;
    r.shots  = shots;
    r.params = params;

    // Fill params defaults if empty
    if (r.params.sessionId.isEmpty())
        r.params.sessionId = "PA_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HHmm");
    if (r.params.shooterId.isEmpty())
        r.params.shooterId = "JohnD";
    if (r.params.shotsScheduled == 0)
        r.params.shotsScheduled = qMax(10, shots.size());

    // Mock ShoQ review
    r.shoq.reviewText =
        "This session showed strong overall performance with consistent accuracy. "
        "Shot placement was notably tight in the A-zone, and split times remained "
        "competitive throughout the drill. Minor inconsistencies in trigger control "
        "were observed in shots 7-9, likely due to fatigue. Overall technique is solid "
        "and reflects disciplined practice habits.";

    r.shoq.insightStrength  = "Consistent A-zone accuracy across all 10 rounds";
    r.shoq.insightFocus     = "Trigger control on rounds 7–9 shows slight deviation";
    r.shoq.insightChallenge = "Maintaining sub-2.0s splits under fatigue conditions";

    r.shoq.metrics = {
        { "Shot Consistency", 92, "Tight grouping throughout" },
        { "Trigger Control",  83, "Minor variation mid-session" },
        { "Shot Placement",   87, "Strong A-zone accuracy" },
        { "Split Time",       91, "Competitive average splits" }
    };

    r.shoq.skillFocusTip =
        "Focus on maintaining grip pressure during rapid follow-up shots. "
        "Consider dry-fire practice for trigger reset consistency.";

    r.shoq.rating     = "A-";
    r.shoq.ratingDesc =
        "Excellent performance with minor areas to refine. "
        "Consistent accuracy and timing place this session in the top tier.";

    return r;
}
