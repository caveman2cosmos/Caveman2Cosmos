#pragma once

// Collect the three existing defense counts in one read-only traversal.
// No answer persists across a move, casualty, role change or a new guard call.
struct CityGuardDefenseSnapshot
{
	int all, cityAI, inCity;
	CityGuardDefenseSnapshot(const CvPlot* center, PlayerTypes owner)
		: all(0), cityAI(0), inCity(0)
	{
		bool countedCenter = false;
		foreach_(const CvPlot* tile, center->rect(2, 2))
		{
			const bool countCenter = tile == center && !countedCenter;
			if (countCenter) countedCenter = true;
			foreach_(const CvUnit* unit, tile->units())
			{
				if (unit->isDead() || owner != NO_PLAYER && unit->getOwner() != owner
					|| !PUF_canDefend(unit, -1, -1, NULL)) continue;
				++all;
				if (PUF_isCityAIType(unit, -1, -1, NULL)) ++cityAI;
				if (countCenter) ++inCity;
			}
		}
		// Also retain plotCount's semantics for a clipped/custom plot range.
		if (!countedCenter)
			inCity = center->plotCount(PUF_canDefend, -1, -1, NULL, owner);
	}
};

// In the city-AI branch the city-role count is irrelevant: the final OR is
// already true. Only decide whether max(danger, all / 2) + offset covers the
// center. Count the center once, reuse it for wrapped center entries, and
// stop as soon as enough nearby defenders make the decision inevitable.
template<class Danger>
inline bool cityAIGuardNeedsInCity(const CvPlot* center, PlayerTypes owner,
	int groupOffset, Danger& danger)
{
	const int inCity = center->plotCount(PUF_canDefend, -1, -1, NULL, owner);
	const int needed = inCity - groupOffset;
	if (needed <= 0 || danger.get() >= needed) return true;
	const int threshold = 2 * needed;
	int all = 0;
	foreach_(const CvPlot* tile, center->rect(2, 2))
	{
		if (tile == center)
		{
			all += inCity;
			if (all >= threshold) return true;
			continue;
		}
		foreach_(const CvUnit* unit, tile->units())
		{
			if (unit->isDead() || owner != NO_PLAYER && unit->getOwner() != owner
				|| !PUF_canDefend(unit, -1, -1, NULL)) continue;
			if (++all >= threshold) return true;
		}
	}
	return false;
}
