#pragma once
#include <algorithm>

// Engine-independent edge arithmetic used by production movement and fixtures.
namespace RailRoadRules
{
    inline int edgeCost(int fromMovement,int fromFlat,int toMovement,int toFlat,int moves,int denominator)
    {
        return std::min(denominator,std::min(std::max(fromMovement,toMovement),std::max(fromFlat,toFlat)*moves));
    }
    inline unsigned int roadHash(unsigned int routeHash)
    {
        return (routeHash<<13)|(routeHash>>19);
    }
    inline unsigned int movementCacheKey(unsigned int key, bool teamRailAccess)
    {
        // Native owner hashes cancel when both endpoints have the same owner.
        // Own and foreign railway edges must still have different cache keys.
        return teamRailAccess ? key ^ 0x9e3779b9u : key;
    }
    inline bool mayUseRail(int plotTeam,int unitTeam)
    {
        return plotTeam>=0 && plotTeam==unitTeam;
    }
}
