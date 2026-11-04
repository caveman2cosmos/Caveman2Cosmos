#pragma once
#include <limits.h>
#include <algorithm>

// Pickup ranks a port by floor(rawValue*1000/(arrivalTurns+3)). Preserve
// traversal-order ties: only a strictly larger result can replace the port.
// This is a bound on the existing score, not a new distance/AI heuristic.
inline int transportPickupPathLimit(int rawValue, int bestValue, int maxTurns)
{
    // Do not introduce a rejection in legacy integer-overflow corner cases.
    if (rawValue <= 0 || rawValue > INT_MAX / 1000 || bestValue <= 0)
        return maxTurns;
    const __int64 turns = ((__int64)rawValue * 1000) / ((__int64)bestValue + 1) - 3;
    return turns < 0 ? -1 : std::min(maxTurns, (int)turns);
}
