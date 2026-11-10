#pragma once
#include "CvEnums.h"

class CvUnit;
class CvPlot;
class CvCity;
class CvSelectionGroup;

namespace StackPromotions
{
    enum Stat { COMBAT, ATTACK, DEFENSE, CITY_ATTACK, CITY_DEFENSE, FIRST_STRIKE_CHANCES,
        WITHDRAWAL, INTERCEPTION, HEAL, NUM_STATS };

    // Transient, read-only derived effects. Never written into unit promotion bits or saves.
    struct Effects
    {
        int stats[NUM_STATS];
        PromotionTypes promotions[32];
        int count;
        Effects();
    };

    bool enabled();
    bool eligible(const CvUnit& unit);
    const Effects& effects(const CvUnit& unit, const CvPlot* at = NULL);
    int value(const CvUnit& unit, Stat stat);
    int versus(const CvUnit& unit, UnitCombatTypes combat);
    int featureAttack(const CvUnit& unit, FeatureTypes feature);
    bool has(const CvUnit& unit, PromotionTypes promotion);
    int healingRate(const CvUnit& unit, const CvPlot* at, int otherwise);
    int limit(TeamTypes team, const CvPlot& plot, DomainTypes domain);

    // Preferences only: no missions, path searches or forced group splitting.
    int mergePreference(const CvUnit& joining, const CvSelectionGroup& target, bool wholeGroup);
    int productionPreference(const CvCity& city, UnitTypes unit);
    int techValue(TechTypes tech);

    void reset();
    void releaseThreadCache();
    void invalidateTeam(TeamTypes team);
    void writeStats();
}
