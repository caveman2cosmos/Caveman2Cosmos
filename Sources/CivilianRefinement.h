#pragma once
#include <cstdlib>
#include <vector>

namespace CivilianRefinement {
inline int mode() {
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* value = std::getenv("C2C_CIVILIAN_REFINE_MODE");
    static const int selected = value ? std::atoi(value) : 7;
    return selected;
#else
    return 7;
#endif
}
inline bool escort() { return (mode() & 1) != 0; }
inline bool rescue() { return (mode() & 2) != 0; }
inline bool routeFailures() { return (mode() & 4) != 0; }

// One unit, fixed flags, one read-only route selection. Only failed queries
// are reused: successful queries still establish the native path state.
// This object dies before/with the selector and never spans world mutations.
class RouteFailures {
    std::vector<int> plots;
public:
    bool contains(int plot) const {
        if (!routeFailures()) return false;
        for (size_t i = 0; i < plots.size(); ++i)
            if (plots[i] == plot) return true;
        return false;
    }
    void remember(int plot) {
        if (routeFailures() && !contains(plot)) plots.push_back(plot);
    }
    void clear() { plots.clear(); }
    template<class Unit, class Plot> bool path(Unit& unit, const Plot* target, int index, int flags) {
        if (contains(index)) return false;
        if (unit.generatePath(target, flags, true)) return true;
        remember(index);
        return false;
    }
};
}
