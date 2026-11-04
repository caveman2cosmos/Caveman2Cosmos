#pragma once
#include <vector>
#include <cstdlib>
#include "CivilianRefinement.h"

// Transient accepted orders, not cached scores or paths. Loading a save or
// copying a unit starts with no commitment; normal AI establishes it again.
namespace CivilianDecision {
enum Kind { NONE, WORKER_ROUTE, WORKER_CONNECTION, SETTLER_ESCORT };
struct State {
    int kind, owner, map, role, type, group, members, origin, target;
    int improvement, route, feature, build, cities, plotUnits, pickup, range;
    bool financial;
    std::vector<int> sites, observations;
    State():kind(NONE),owner(-1),map(-1),role(-1),type(-1),group(-1),members(0),
        origin(-1),target(-1),improvement(-1),route(-1),feature(-1),build(-1),
        cities(0),plotUnits(0),pickup(0),range(0),financial(false){}
};
inline bool enabled() {
#ifdef C2C_RECALCULATED_BENCHMARK
    const char* value = getenv("C2C_CIVILIAN_DECISION_CACHE");
    return !value || value[0] != '0';
#else
    return true;
#endif
}
}
