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

    // AI planning always omits derived Aid, including nested combat estimates.
    // Only explicit mechanical/UI roots can restore exact effects inside planning.
    // A scope never stores its approximation in the exact effect cache.
    class EvaluationScope
    {
        bool previous;
        bool previousPlanning;
        EvaluationScope(const EvaluationScope&);
        EvaluationScope& operator=(const EvaluationScope&);
    public:
        explicit EvaluationScope(bool exact);
        ~EvaluationScope();
    };
    class MechanicsScope
    {
        MechanicsScope(const MechanicsScope&);
        MechanicsScope& operator=(const MechanicsScope&);
    public:
        MechanicsScope();
        ~MechanicsScope();
    };
    bool detailed();
    bool detailedForCombat(bool concreteOpponent);
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
