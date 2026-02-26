#include "DataModels.h"

#include "DataModels.h"

QMap<QString, Category> DataModels::getCategories()
{
    QMap<QString, Category> categories;

    Category handgun;
    handgun.id = "handgun";
    handgun.label = "Handgun";

    Caliber cal9mm;
    cal9mm.id = "9mm";
    cal9mm.label = "9mm Luger";
    cal9mm.profiles = {
        {"hg_9mm_compact", "Compact 9mm", "G19 / P320C / M&P9C"},
        {"hg_9mm_full", "Full-Size 9mm", "G17 / VP9 / M&P9"},
        {"hg_9mm_sub", "Subcompact 9mm", "G43X / P365 / Hellcat"}};

    Caliber cal45acp;
    cal45acp.id = ".45ACP";
    cal45acp.label = ".45 ACP";
    cal45acp.profiles = {
        {"hg_45_full", "Full-Size .45", "1911 / Glock 21 / similar"},
        {"hg_45_compact", "Compact .45", "G30 / XD-M / similar"}};

    Caliber cal38_357;
    cal38_357.id = ".38_357";
    cal38_357.label = ".38 / .357 Revolver";
    cal38_357.profiles = {
        {"hg_38_snub", "Snub Revolver", "2\" barrel defensive"},
        {"hg_38_full", "Service Revolver", "4\" barrel / duty"}};

    handgun.calibers["9mm"] = cal9mm;
    handgun.calibers[".45ACP"] = cal45acp;
    handgun.calibers[".38_357"] = cal38_357;
    categories["handgun"] = handgun;

    Category rifle;
    rifle.id = "rifle";
    rifle.label = "Rifle";

    Caliber cal556_223;
    cal556_223.id = "5.56_223";
    cal556_223.label = "5.56 / .223";
    cal556_223.profiles = {
        {"rf_ar15_16", "AR-15 16\"", "Standard carbine / mid-length"}};

    rifle.calibers["5.56_223"] = cal556_223;
    categories["rifle"] = rifle;

    Category pcc;
    pcc.id = "pcc";
    pcc.label = "PCC";

    Caliber pcc9mm;
    pcc9mm.id = "9mmPCC";
    pcc9mm.label = "9mm PCC";
    pcc9mm.profiles = {
        {"pcc_9mm", "9mm PCC", "AR-pattern / Scorpion / CX4"}};

    pcc.calibers["9mmPCC"] = pcc9mm;
    categories["pcc"] = pcc;

    return categories;
}

QVector<int> DataModels::getDistances()
{
    return {7, 10, 12, 15, 20, 25};
}

QVector<Target> DataModels::getTargets()
{
    return {
        {"b8_bullseye", "B-8 Repair Center", "25-yd Timed/Rapid Fire"},
        {"bullseye", "10-Ring Bullseye", "Precision Ring Target"}};
}

QVector<Drill> DataModels::getDrills()
{
    return {
        {"free_fire", "Free Fire", "Untimed scoring & grouping"},
        {"bill_drill", "Bill Drill", "6 shots A-Zone (Fast)"},
        // {"ten_in_ten", "10-in-10", "10 shots in 10 seconds"},
        // {"ragged_hole", "Ragged Hole", "5 Shots, 1 Hole Precision"}
    };
}

QVector<RecentSession> DataModels::getRecentSessions()
{
    return {
        {"handgun", "9mm", "hg_9mm_compact"},
        {"rifle", "5.56_223", "rf_ar15_16"}};
}