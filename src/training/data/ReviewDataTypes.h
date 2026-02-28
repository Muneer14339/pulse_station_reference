#pragma once
// src/training/data/ReviewDataTypes.h
// ── Pure data layer — no Qt widgets, no GUI includes ──────────────────────────
#include <QString>
#include <QVector>
#include <QDateTime>

// ─── Single shot record ────────────────────────────────────────────────────────
struct ShotRecord {
    int     number    = 0;
    int     score     = 0;
    double  splitTime = -1.0;   // -1 = first shot (no split)
    QString direction;          // "center", "high-left", etc. — optional
    QString imagePath;          // optional screenshot path
};

// ─── Session parameters (what was configured before the session) ───────────────
struct SessionParameters {
    QString sessionId;          // e.g. "PA_2025-05-05_1210"
    QString shooterId;          // e.g. "JohnD"
    QString firearm;            // e.g. "Pistol, Beretta"
    QString weaponType;         // categoryId from SessionState
    int     distance     = 0;   // yards
    int     shotsScheduled = 0;
    QString drillId;
};

// ─── ShoQ AI review data ──────────────────────────────────────────────────────
struct ShoQMetric {
    QString name;
    int     pct    = 0;
    QString desc;
};

struct ShoQReviewData {
    QString             reviewText;
    QString             insightStrength;
    QString             insightFocus;
    QString             insightChallenge;
    QVector<ShoQMetric> metrics;
    QString             skillFocusTip;
    QString             rating;         // "A-"
    QString             ratingDesc;
};

// ─── Full session result (aggregates everything) ──────────────────────────────
struct SessionResult {
    QVector<ShotRecord> shots;
    SessionParameters   params;
    ShoQReviewData      shoq;

    // Computed helpers
    int  totalScore()   const;
    int  shotsFired()   const { return shots.size(); }
    int  shotsMissing() const { return qMax(0, params.shotsScheduled - shotsFired()); }
    double avgScore()   const;
    double bestSplit()  const;
    double avgSplit()   const;

    /** Build a result from live shot data + params; ShoQ data is mocked. */
    static SessionResult buildMockResult(const QVector<ShotRecord>& shots,
                                         const SessionParameters&   params);
};
