#pragma once

// Engine-independent contracts shared by the DLL and isolated regression tests.
namespace StackPromotionMath
{
    // Aid is already shared across selection groups on one plot. Finish a
    // requested legal local merge before travelling for a small aid score.
    // With the option disabled the original score ordering is unchanged.
    inline bool preferLocalGroup(bool enabled, bool local, bool bestLocal, int value, int bestValue)
    {
        if (enabled && local != bestLocal) return local;
        return value < bestValue;
    }

    inline int aidTier(long long support, long long receiver)
    {
        if (receiver <= 0 || support <= 0) return 0;
        if (support > receiver * 2) return 3;
        if (support > receiver) return 2;
        return support * 2 > receiver ? 1 : 0;
    }

    inline int overcrowdedTier(int count, int limit)
    {
        const int extra = count - limit;
        return extra <= 0 ? 0 : extra <= 2 ? 1 : extra <= 5 ? 2 : extra <= 9 ? 3 : 4;
    }

    inline int healWithCongestion(int otherwise, int penalty)
    {
        // Congestion cannot grant healing where the normal rules forbid it.
        if (otherwise <= 0) return 0;
        return otherwise + penalty > 0 ? otherwise + penalty : 1;
    }
}
