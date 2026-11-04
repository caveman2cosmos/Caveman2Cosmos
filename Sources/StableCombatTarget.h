#pragma once
#include <map>

// A pointer-keyed container may enumerate equal scores in allocator order.
// Resolve those ties with saved identities; do not draw additional randoms.
namespace StableCombatTarget
{
    template<class Unit> Unit* select(const std::map<Unit*, int>& ranks)
    {
        Unit* best = NULL;
        int bestScore = 0;
        for (typename std::map<Unit*, int>::const_iterator it = ranks.begin(); it != ranks.end(); ++it)
        {
            Unit* candidate = it->first;
            if (it->second > bestScore || (best && it->second == bestScore &&
                (candidate->getOwner() < best->getOwner() ||
                (candidate->getOwner() == best->getOwner() && candidate->getID() < best->getID()))))
            {
                best = candidate;
                bestScore = it->second;
            }
        }
        return best;
    }
}
