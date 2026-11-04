#pragma once

// The route finder's original non-root predicate. Share the exact rule with
// read-only planning; do not replace it with trade groups or route tiers.
namespace NativeRoutePolicy {
template<class Player,class Plot>
bool accepts(const Player& player,const Plot* plot) {
    const RouteTypes best=player.getBestRoute(plot);
    return (!plot->isOwned() || plot->getTeam()==player.getTeam())
        && plot->getRouteTypeForBuild(best)==best;
}
}
