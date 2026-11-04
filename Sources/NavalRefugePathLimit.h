#pragma once
#include <algorithm>

// Integer-score upper bounds only. A path that cannot strictly beat the
// incumbent is irrelevant; tie order and the original safety checks remain.
inline int navalCityPathLimit(int baseValue, int bestValue, int range)
{
    if (baseValue <= bestValue) return -1;
    if (bestValue <= 0) return range;
    return std::min(range, baseValue / (bestValue + 1) - 1);
}

inline int navalRetreatPathLimit(int range, int pass, int bestValue)
{
    const int acceptedRange = pass == 2 ? 1 : range;
    // The settler transport multiplier is at least one, so raw path turns
    // are a lower bound for its final score too. Zero-turn paths still work.
    return std::min(acceptedRange, bestValue - 1);
}

// AI_safety adds at most 49 random points to a non-current tile. Damage and
// travel can only reduce this optimistic score; danger cannot make it win.
inline int safetyScorePathLimit(int baseValue, bool current, int damagePercent,
    int terrainDamage, int bestValue, int range)
{
    int maximum = baseValue + (current ? 0 : 49);
    if (terrainDamage > 0)
    {
        maximum *= std::max(1, 100 - damagePercent - terrainDamage);
        maximum /= 200;
    }
    if (maximum * 5 / 4 <= bestValue) return -1;
    if (bestValue <= 0) return range;
    return std::min(range, maximum * 5 / (bestValue + 1) - 4);
}
