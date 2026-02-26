#pragma once
#include <QString>
#include <QVector>
#include <QMap>

// ─────────────────────────────────────────────────────────────────────────────
//  Data structures
// ─────────────────────────────────────────────────────────────────────────────

struct Profile {
    QString id;
    QString label;
    QString desc;
};

struct Caliber {
    QString id;
    QString label;
    QVector<Profile> profiles;
};

struct Category {
    QString id;
    QString label;
    QMap<QString, Caliber> calibers;
};

struct Target {
    QString id;
    QString label;
    QString desc;
};

struct Drill {
    QString id;
    QString label;
    QString desc;
};

struct RecentSession {
    QString categoryId;
    QString caliberId;
    QString profileId;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Static data provider
// ─────────────────────────────────────────────────────────────────────────────

class DataModels {
public:
    static QMap<QString, Category>  getCategories();
    static QVector<int>             getDistances();
    static QVector<Target>          getTargets();
    static QVector<Drill>           getDrills();
    static QVector<RecentSession>   getRecentSessions();
};
