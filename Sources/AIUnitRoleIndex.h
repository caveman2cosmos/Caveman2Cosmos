#pragma once
#include <vector>

class CvUnit;

// Derived, non-serialized candidates. Movement/cargo/diplomacy are deliberately
// not cached: callers apply those live predicates after this stable role filter.
class AIUnitRoleIndex
{
    int m_map;
    bool m_valid;
    std::vector<CvUnit*> m_roles[NUM_UNITAI_TYPES];
public:
    AIUnitRoleIndex() : m_map(-1), m_valid(false) {}
    void invalidate() { m_valid = false; }
    bool ready(int map) const { return m_valid && m_map == map; }
    void begin(int map)
    {
        m_map = map;
        m_valid = false;
        for (int i = 0; i < NUM_UNITAI_TYPES; ++i)
            m_roles[i].erase(m_roles[i].begin(), m_roles[i].end());
    }
    void append(int role, CvUnit* unit)
    {
        if (role >= 0 && role < NUM_UNITAI_TYPES) m_roles[role].push_back(unit);
    }
    void complete() { m_valid = true; }
    const std::vector<CvUnit*>& get(int role) const
    {
        static const std::vector<CvUnit*> empty;
        return role >= 0 && role < NUM_UNITAI_TYPES ? m_roles[role] : empty;
    }
};
