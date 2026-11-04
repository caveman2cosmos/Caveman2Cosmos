#pragma once
#include <vector>
class CvUnit;

// Ordered, derived membership only. Ready/moved/cargo/health/range/enemy tests
// stay live. Exact Aid and commander contexts use the complete ordered list.
class AirInterceptionIndex
{
    int m_map;
    bool m_valid;
    std::vector<CvUnit*> m_all, m_intrinsic;
public:
    AirInterceptionIndex() : m_map(-1), m_valid(false) {}
    void invalidate() { m_valid = false; }
    bool ready(int map) const { return m_valid && m_map == map; }
    void begin(int map)
    {
        m_map = map; m_valid = false;
        m_all.clear(); m_intrinsic.clear();
    }
    void append(CvUnit* unit, bool intrinsic)
    {
        m_all.push_back(unit);
        if (intrinsic) m_intrinsic.push_back(unit);
    }
    void complete() { m_valid = true; }
    const std::vector<CvUnit*>& get(bool intrinsicOnly) const
    { return intrinsicOnly ? m_intrinsic : m_all; }
};
