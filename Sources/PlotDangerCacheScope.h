#pragma once

#include <climits>

// Boolean danger is not a defender-adjusted numeric danger score. Only reuse
// its answers inside an explicitly read-only search, never across unit moves,
// reveals, promotions, diplomacy, turn changes or hypothetical-unit teardown.
// Like PathHeuristicContext, this context belongs to the serial path generator.
class PlotDangerCacheScope
{
    struct State
    {
        unsigned int next;
        PlotDangerCacheScope* current;
        State() : next(0), current(0) {}
    };
    static State& state() { static State value; return value; }
    PlotDangerCacheScope* m_previous;
    unsigned int m_token;
    int m_player;
public:
    explicit PlotDangerCacheScope(int player, bool enabled = true)
        : m_previous(state().current), m_token(0), m_player(player)
    {
        // Do not recycle tokens on overflow: old plots may retain any token.
        // Nested searches inside a read-only candidate selection see the same
        // world. Reuse the owner's token; other owners and disabled frames stay isolated.
        if (enabled && m_previous && m_previous->m_token && m_previous->m_player == player)
            m_token = m_previous->m_token;
        else if (enabled && state().next != UINT_MAX) m_token = ++state().next;
        state().current = this;
    }
    ~PlotDangerCacheScope() { state().current = m_previous; }
    static unsigned int token()
    {
        return state().current ? state().current->m_token : 0;
    }
    static bool matches(int player)
    {
        return token() != 0 && state().current->m_player == player;
    }
private:
    PlotDangerCacheScope(const PlotDangerCacheScope&);
    PlotDangerCacheScope& operator=(const PlotDangerCacheScope&);
};
