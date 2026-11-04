#pragma once

// Local threshold query, not a persistent garrison cache. A deficient city
// receives its exact count; an already sufficient city needs no full scan.
inline int cityDefendersUpTo(const CvPlot* plot, PlayerTypes owner, int limit)
{
    int count = 0;
    if (limit <= 0) return count;
    foreach_(const CvUnit* unit, plot->units())
    {
        if (!unit->isDead() && unit->getOwner() == owner
            && unit->AI_getUnitAIType() == UNITAI_CITY_DEFENSE)
        {
            if (++count >= limit) break;
        }
    }
    return count;
}
