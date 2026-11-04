#pragma once
#include <vector>
#include <cstddef>

// Shared map topology, NOT movement permissions or movement costs. Roads,
// terrain, units, diplomacy and revelation are deliberately not stored here.
// Every exact path callback still runs with the querying group's live state.
// Keeping direction slots (including nulls and duplicate wrapped neighbours)
// preserves the pathfinder's discovery order and edge-direction bitmasks.
template<class Plot> class PathGeography
{
    struct Row { Plot* adjacent[8]; };
    std::vector<Row> rows;
public:
    bool empty() const { return rows.empty(); }
    std::size_t bytes() const { return rows.capacity() * sizeof(Row); }
    int size() const { return (int)rows.size(); }
    Plot* const* neighbors(int plot) const { return rows[plot].adjacent; }
    Plot* neighbor(int plot, int direction) const
    { return rows[plot].adjacent[direction]; }

    // Map storage must outlive every row. Call clear before deleting plots,
    // including same-sized save reloads and map regeneration. Not serialized.
    void clear() { std::vector<Row>().swap(rows); }

    template<class Graph> void build(const Graph& graph)
    {
        // Publish only the complete table. No unit/player/team-specific copies.
        std::vector<Row> next(graph.size());
        for(int p=0; p<graph.size(); ++p)
            for(int d=0; d<8; ++d)
                next[p].adjacent[d]=graph.neighbor(p,d);
        rows.swap(next);
    }
};
