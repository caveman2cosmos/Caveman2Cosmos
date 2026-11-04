#pragma once
#include <vector>

// Only static candidate membership, never occupants, orders or danger.
class AIPlotCandidateIndex
{
    int m_map;
    bool m_valid;
    std::vector<int> m_indices;
public:
    AIPlotCandidateIndex() : m_map(-1), m_valid(false) {}
    void invalidate() { m_valid = false; }
    bool ready(int map) const { return m_valid && m_map == map; }
    void begin(int map)
    {
        m_map = map; m_valid = false;
        m_indices.erase(m_indices.begin(), m_indices.end());
    }
    void append(int index) { m_indices.push_back(index); }
    void complete() { m_valid = true; }
    const std::vector<int>& get() const { return m_indices; }
};
