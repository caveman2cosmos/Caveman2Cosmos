#pragma once

class CvPlot;

// Eight adjacent scores, local to one naval patrol; no heap or persistent state.
struct PatrolCandidate
{
    const CvPlot* plot;
    int value, order;
    bool operator<(const PatrolCandidate& other) const
    {
        return value != other.value ? value > other.value : order < other.order;
    }
};
