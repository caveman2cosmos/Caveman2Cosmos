#pragma once
#include "OffensiveStackBalance.h"
#include "CvPlot.h"

// Army commands and individual role handlers must agree about the assignment.
// Inspect live roles/promotions; no per-turn or persistent recruitment cache.
namespace ArmyRecruitment
{
inline bool eligible(const CvSelectionGroup* group)
{
    if (group == NULL || group->getHeadUnit() == NULL || group->isHuman()
        || group->getDomainType() != DOMAIN_LAND || group->getHeadUnit()->isCargo()) return false;
    const UnitAITypes role = group->getHeadUnitAI();
    if (role != UNITAI_ATTACK && role != UNITAI_ATTACK_CITY
        && role != UNITAI_ATTACK_CITY_LEMMING && role != UNITAI_COUNTER
        && role != UNITAI_RESERVE && role != UNITAI_PARADROP) return false;
    if (group->plot() != NULL && group->AI_isCityGarrison(group->plot()->getPlotCity())) return false;
    switch (group->AI_getMissionAIType())
    {
    case MISSIONAI_GUARD_CITY: case MISSIONAI_GUARD_BONUS:
    case MISSIONAI_GUARD_TRADE_NET: case MISSIONAI_GUARD_SPY:
    case MISSIONAI_LOAD_SETTLER: case MISSIONAI_LOAD_ASSAULT: case MISSIONAI_LOAD_SPECIAL:
        return false;
    default: break;
    }
    const OffensiveStackBalance::Counts counts = OffensiveStackBalance::count(*group);
    return counts.attackers > 0 && counts.defenders <= counts.attackers;
}
}
