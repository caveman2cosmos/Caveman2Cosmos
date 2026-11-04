#pragma once
#include <limits.h>

#ifdef C2C_PATH_OPPORTUNITY_PERF
namespace PathOpportunityPerf { int defenderOptimizationStage(); }
#endif

// The caller already rejects arrivals above this bound. Supply it to the
// search as well; retain the final acceptance check and all defense rules.
inline int defenderCityPathLimit(int acceptedTurns)
{
#ifdef C2C_PATH_OPPORTUNITY_PERF
    if (PathOpportunityPerf::defenderOptimizationStage() < 1) return INT_MAX;
#endif
    return acceptedTurns;
}

// Minimum fort defense ranks candidates by floor(1000/(turns+2)).
// A later candidate must strictly beat the current best; ties keep the
// existing traversal-order winner. Derive the exact integer turn bound.
inline int defenderFortPathLimit(int bestValue)
{
#ifdef C2C_PATH_OPPORTUNITY_PERF
    if (PathOpportunityPerf::defenderOptimizationStage() < 2) return INT_MAX;
#endif
    if (bestValue <= 0) return INT_MAX;
    return (int)(1000 / ((__int64)bestValue + 1)) - 2;
}

// Local defensive positions use floor(baseValue*1000/(turns+2)).
inline int defenderPositionPathLimit(int baseValue, int bestValue)
{
    if (baseValue <= 0) return -1;
    if (bestValue <= 0) return INT_MAX;
    const __int64 limit = ((__int64)baseValue * 1000) / ((__int64)bestValue + 1) - 2;
    return (int)(limit > INT_MAX ? INT_MAX : limit);
}
