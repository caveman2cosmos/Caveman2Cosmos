#pragma once

// XML-only data: first priority-one status in each status's line. Rebuild with
// the existing status list after XML loading; never cache a unit's decision.
template<class Globals>
void cacheStatusPromotionDefaults(const Globals& globals, std::vector<PromotionTypes>& defaults)
{
	const int count = globals.getNumStatusPromotions();
	defaults.assign(count, NO_PROMOTION);
	for (int i = 0; i < count; ++i)
	{
		const CvPromotionInfo& status = globals.getPromotionInfo(globals.getStatusPromotion(i));
		for (int j = 0; j < count; ++j)
		{
			const PromotionTypes candidate = globals.getStatusPromotion(j);
			const CvPromotionInfo& info = globals.getPromotionInfo(candidate);
			if (status.getPromotionLine() == info.getPromotionLine() && info.getLinePriority() == 1)
			{
				defaults[i] = candidate;
				break;
			}
		}
	}
}
