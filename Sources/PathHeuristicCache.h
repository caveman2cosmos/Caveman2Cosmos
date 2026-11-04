#pragma once

// A path search only reads the group. Cache its minimum maximum movement
// inside that one query, never across moves, promotions or group changes.
// Nested/hypothetical searches restore the previous context on exit.
class PathHeuristicContext
{
    const void* m_group;
    PathHeuristicContext* m_previous;
    int m_minimum;
    bool m_known, m_enabled;
    bool m_memo;
    enum { SLOT_COUNT = 16 };
    struct Entry { int distance, heuristic, limit; } m_entries[SLOT_COUNT];
    static PathHeuristicContext*& current()
    {
        static PathHeuristicContext* context = 0;
        return context;
    }
public:
    explicit PathHeuristicContext(const void* group, bool enabled = true, bool memo = false)
        : m_group(group), m_previous(current()), m_minimum(0),
          m_known(false), m_enabled(enabled), m_memo(enabled && memo)
    {
        if (m_memo) for (int i=0;i<SLOT_COUNT;++i) m_entries[i].distance=-1;
        current() = this;
    }
    ~PathHeuristicContext() { current() = m_previous; }
    static bool lookup(const void* group, int& minimum)
    {
        const PathHeuristicContext* context = current();
        if (!context || !context->m_enabled || context->m_group != group || !context->m_known) return false;
        minimum = context->m_minimum;
        return true;
    }
    static void remember(const void* group, int minimum)
    {
        PathHeuristicContext* context = current();
        if (context && context->m_enabled && context->m_group == group)
        { context->m_minimum = minimum; context->m_known = true; }
    }
    // The caller enables this only for land heuristics, whose two outputs
    // depend on distance and the fixed group/player parameters of this query.
    // Full distance keys make collisions misses; no allocation or map-size cap.
    static bool lookupHeuristic(const void* group, int distance, int& heuristic, int& limit, bool& enabled)
    {
        const PathHeuristicContext* context=current();
        enabled=context && context->m_memo && context->m_group==group && distance>=0;
        if (!enabled) return false;
        const Entry& entry=context->m_entries[distance & (SLOT_COUNT-1)];
        if (entry.distance!=distance) return false;
        heuristic=entry.heuristic; limit=entry.limit;
        return true;
    }
    static void rememberHeuristic(const void* group, int distance, int heuristic, int limit)
    {
        PathHeuristicContext* context=current();
        if (!context || !context->m_memo || context->m_group!=group || distance<0) return;
        Entry& entry=context->m_entries[distance & (SLOT_COUNT-1)];
        entry.distance=distance; entry.heuristic=heuristic; entry.limit=limit;
    }
private:
    PathHeuristicContext(const PathHeuristicContext&);
    PathHeuristicContext& operator=(const PathHeuristicContext&);
};
