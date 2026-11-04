#pragma once
#include "TargetDecisionDiagnostics.h"

// A conservative existence check, not a new target selector. Every successful
// old pass needs a hostile city adjacent to a plot within this step radius.
// Keep the original flood/hash order and scoring when any such city exists.
inline bool hasPotentialNavalBombardCity(const CvUnit& unit, int range)
{
    TARGET_DIAG_TIMER(BOMBARD_PREFLIGHT);
    TARGET_DIAG_COUNT(BOMBARD_CALLS,1);
    if (range < 0 || range == MAX_INT) return true;
    TARGET_DIAG_LOCAL(visited);
    for (int i=0;i<MAX_PLAYERS;++i)
    {
        const CvPlayer& player=GET_PLAYER((PlayerTypes)i);
        foreach_(const CvCity* city, player.cities())
        {
            TARGET_DIAG_VISIT(visited);
            if (stepDistance(unit.getX(),unit.getY(),city->getX(),city->getY()) <= range+1
                && unit.isEnemy(city->getTeam()))
            {
                TARGET_DIAG_COUNT(BOMBARD_CITIES,visited);
                return true;
            }
        }
    }
    TARGET_DIAG_COUNT(BOMBARD_CITIES,visited);
    TARGET_DIAG_COUNT(BOMBARD_REJECTS,1);
    return false;
}
