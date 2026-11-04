#pragma once

#include "CvUnit.h"
#include "CvSelectionGroup.h"

// AI recruitment policy, not a restriction on the human's group controls.
// No persistent state: deaths, promotions and arriving reinforcements are seen
// by the final canJoinGroup check as well as the cheap candidate preflights.
namespace OffensiveStackBalance
{
inline bool offensive(UnitAITypes role)
{
    return role == UNITAI_ATTACK || role == UNITAI_ATTACK_CITY
        || role == UNITAI_ATTACK_CITY_LEMMING || role == UNITAI_PARADROP
        || role == UNITAI_PILLAGE || role == UNITAI_ATTACK_SEA;
}

inline bool support(UnitAITypes role)
{
    return role == UNITAI_HEALER || role == UNITAI_HEALER_SEA
        || role == UNITAI_GENERAL || role == UNITAI_GREAT_ADMIRAL
        || role == UNITAI_PROPERTY_CONTROL || role == UNITAI_PROPERTY_CONTROL_SEA
        || role == UNITAI_SEE_INVISIBLE || role == UNITAI_SEE_INVISIBLE_SEA;
}

struct Counts
{
    int attackers, defenders;
    Counts() : attackers(0), defenders(0) {}
    void add(const CvUnit& unit)
    {
        if (support(unit.AI_getUnitAIType()) || !unit.canFight()) return;
        if (unit.canAttack()) ++attackers;
        else if (unit.isOnlyDefensive()) ++defenders;
    }
};

inline Counts count(const CvSelectionGroup& group)
{
    Counts result;
    foreach_(const CvUnit* unit, group.units()) result.add(*unit);
    return result;
}

inline bool allows(const Counts& present, const Counts& added)
{
    // Always permit attackers/support to repair an old, over-defensive army.
    return added.defenders == 0
        || present.defenders + added.defenders <= present.attackers + added.attackers;
}

inline bool allows(const CvUnit& unit, const CvSelectionGroup& target, bool wholeGroup)
{
    if (!offensive(target.getHeadUnitAI()) || unit.getOwner() == NO_PLAYER
        || unit.isHuman() || unit.getGroup() == &target) return true;
    Counts added;
    if (wholeGroup && unit.getGroup() != NULL) added = count(*unit.getGroup());
    else added.add(unit);
    if (added.defenders == 0) return true;
    return allows(count(target), added);
}
}
