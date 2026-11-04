#pragma once

// Experimental AI allocation policy. All amounts use the existing city AI's
// signed, weighted ten-turn horizon, not a new property simulation.
#include <vector>
#include <algorithm>

namespace CityPropertyDispatch
{
    struct Budget
    {
        std::vector<int> need;
        std::vector<int> present;
        std::vector<int> minimum;
        explicit Budget(int count = 0) : need(count, 0), present(count, 0), minimum(count, 0) {}
    };

    inline bool useful(const Budget& receiver, const std::vector<int>& mobile)
    {
        for (size_t p = 0; p < receiver.need.size(); ++p)
            if (receiver.need[p] > 0 && mobile[p] > 0) return true;
        return false;
    }

    inline bool canRelease(const Budget& donor, const std::vector<int>& current)
    {
        for (size_t p = 0; p < donor.need.size(); ++p)
        {
            if (current[p] > 0 && (donor.present[p] <= donor.minimum[p]
                || (__int64)donor.need[p] + (__int64)current[p] * 10 > 0)) return false;
        }
        return true;
    }

    inline void release(Budget& donor, const std::vector<int>& current)
    {
        for (size_t p = 0; p < donor.need.size(); ++p)
        {
            donor.need[p] = (int)std::max<__int64>(-1000000000,
                std::min<__int64>(1000000000, (__int64)donor.need[p] + (__int64)current[p] * 10));
            if (current[p] > 0) --donor.present[p];
        }
    }

    inline void reserve(Budget& receiver, const std::vector<int>& mobile, int turns = 0)
    {
        // Assistance arriving after the forecast horizon cannot solve today's need.
        const int activeTurns = std::max(0, 10 - turns);
        for (size_t p = 0; p < receiver.need.size(); ++p)
            receiver.need[p] = (int)std::max<__int64>(-1000000000,
                std::min<__int64>(1000000000, (__int64)receiver.need[p] - (__int64)mobile[p] * activeTurns));
    }
}
