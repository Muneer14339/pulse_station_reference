#include "DataModels.h"

QMap<QString, Category> DataModels::getCategories() {
    QMap<QString, Category> categories;
    
    Category handgun;
    handgun.id = "handgun";
    handgun.label = "Handgun";
    
    Caliber cal9mm;
    cal9mm.id = "9mm";
    cal9mm.label = "9mm";
    cal9mm.profiles = {
        {"standard", "Standard", "Mid-weight ball ammo"},
        {"jhp", "JHP / Defense", "Heavier defensive rounds"},
        {"comp", "Competition", "Light/fast loads"}
    };
    
    Caliber cal45acp;
    cal45acp.id = "45acp";
    cal45acp.label = ".45 ACP";
    cal45acp.profiles = {
        {"standard", "Standard", "230gr ball"},
        {"jhp", "JHP / Defense", "Defensive hollow points"}
    };
    
    handgun.calibers["9mm"] = cal9mm;
    handgun.calibers["45acp"] = cal45acp;
    categories["handgun"] = handgun;
    
    Category rifle;
    rifle.id = "rifle";
    rifle.label = "Rifle";
    
    Caliber cal223;
    cal223.id = "223";
    cal223.label = "5.56 / .223";
    cal223.profiles = {
        {"m855", "M855 / 62gr", "Standard military"},
        {"match", "Match / 77gr", "Precision"}
    };
    
    Caliber cal308;
    cal308.id = "308";
    cal308.label = ".308 Win";
    cal308.profiles = {
        {"standard", "Standard", "150-168gr"},
        {"match", "Match", "175+ grain precision"}
    };
    
    rifle.calibers["223"] = cal223;
    rifle.calibers["308"] = cal308;
    categories["rifle"] = rifle;
    
    Category pcc;
    pcc.id = "pcc";
    pcc.label = "PCC";
    
    Caliber pcc9mm;
    pcc9mm.id = "9mm";
    pcc9mm.label = "9mm PCC";
    pcc9mm.profiles = {
        {"standard", "Standard", "Carbine length"},
        {"comp", "Competition", "Race gun setup"}
    };
    
    pcc.calibers["9mm"] = pcc9mm;
    categories["pcc"] = pcc;
    
    return categories;
}

QVector<int> DataModels::getDistances() {
    return {5, 7, 10, 15, 25};
}

QVector<Target> DataModels::getTargets() {
    return {
        {"ipsc", "IPSC", "Standard competition silhouette"},
        {"bullseye", "Bullseye", "Concentric rings for precision"},
        {"steel", "Steel Plate", "Quick reactive feedback"}
    };
}

QVector<Drill> DataModels::getDrills() {
    return {
        {"grouping", "Grouping", "Tight clusters, pure accuracy"},
        {"cadence", "Cadence Fire", "Controlled rapid fire"},
        {"transitions", "Transitions", "Multiple targets, switching"}
    };
}

QVector<RecentSession> DataModels::getRecentSessions() {
    return {
        {"handgun", "9mm", "standard"},
        {"rifle", "223", "m855"}
    };
}