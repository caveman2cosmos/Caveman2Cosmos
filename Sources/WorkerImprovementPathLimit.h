#pragma once
#include <limits.h>
#include <algorithm>

// Strict > ranking: floor(numerator/(turns+1))*afterDivision must beat best.
// Optimistic bounds may keep losers, but must never discard a possible winner.
inline int workerImprovementPathLimit(__int64 numerator,int afterDivision,int best)
{
 if(best<0)return INT_MAX;
 if(numerator<=0 || afterDivision<=0)return -1;
 const __int64 needed=(__int64)best/afterDivision+1;
 const __int64 limit=numerator/needed-1;
 return limit<0 ? -1 : (limit>=INT_MAX ? INT_MAX : (int)limit);
}

// ImproveBonus uses a special zero-turn boost, then max(1,rawValue/turns).
// The minimum score of one means no distance pruning is valid before a winner.
inline int workerServiceBonusPathLimit(int rawValue,int best,int allowedTurns)
{
 const __int64 zeroValue=(__int64)std::max(1,rawValue)*3/2;
 if(best<1)return allowedTurns;
 const int limit=rawValue>0 ? (int)((__int64)rawValue/((__int64)best+1)) : 0;
 if(limit==0 && zeroValue<=best)return -1;
 return std::min(allowedTurns,limit);
}
