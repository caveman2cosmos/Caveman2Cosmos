#pragma once
#include <climits>

// Bound the existing nested integer score, preserving truncation and strict
// first-on-tie selection. Unknown values and overflow-prone inputs stay unbound.
inline int constructionWinningPathLimit(int value, int population, int bestScore)
{
    if (value <= 0 || population < 0 || bestScore < 0) return INT_MAX;
    const __int64 numerator = (__int64)value * 5;
    const __int64 scale = (__int64)population + 5;
    if (numerator > INT_MAX || numerator * scale > INT_MAX) return INT_MAX;
    const __int64 needed = (10 * ((__int64)bestScore + 1) + scale - 1) / scale;
    const __int64 limit = numerator / needed - 4;
    return limit < 0 ? -1 : (limit > INT_MAX ? INT_MAX : (int)limit);
}
