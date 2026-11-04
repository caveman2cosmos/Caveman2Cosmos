#pragma once

class CvPlot;

// One AI_anyAttack preflight and selector, before any orders are issued.
// Existing group odds caching remains authoritative; this also avoids repeated
// target counts, thresholds and eligibility checks. Never survives a decision.
class AttackCandidateMemo
{
    struct Entry { const CvPlot* plot; int value, threshold; };
    enum { CAPACITY = 8 };
    Entry m_values[CAPACITY];
    int m_size;
public:
    AttackCandidateMemo() : m_size(0) {}
    void remember(const CvPlot* plot, int value, int threshold)
    {
        // A full memo only means recomputation; never discard a candidate.
        if (m_size == CAPACITY) return;
        Entry& entry = m_values[m_size++];
        entry.plot = plot; entry.value = value; entry.threshold = threshold;
    }
    bool contains(const CvPlot* plot) const
    {
        for (int i = 0; i < m_size; ++i) if (m_values[i].plot == plot) return true;
        return false;
    }
    bool get(const CvPlot* plot, int& value, int& threshold) const
    {
        for (int i = 0; i < m_size; ++i) if (m_values[i].plot == plot)
        { value = m_values[i].value; threshold = m_values[i].threshold; return true; }
        return false;
    }
};
