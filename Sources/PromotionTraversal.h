#pragma once
#include <vector>

namespace PromotionTraversal
{
	// The unit's sparse map is the authoritative ownership index. Query it live
	// after each callback: removing the current promotion or adding a later one
	// must be visible without another cache, allocation or invalidation scheme.
	template<class Map>
	int owned(Map& promotions, int cursor, int limit, bool descending)
	{
		typename Map::iterator it;
		if (descending)
		{
			it = promotions.lower_bound(static_cast<typename Map::key_type>(cursor == -1 ? limit : cursor));
			while (it != promotions.begin())
			{
				--it;
				if (it->first < 0) break;
				if (it->second.m_bHasPromotion) return it->first;
				if (promotions.size() > 32 && it->second.Empty())
				{
					typename Map::iterator empty = it++;
					promotions.erase(empty);
				}
			}
		}
		else
		{
			it = promotions.upper_bound(static_cast<typename Map::key_type>(cursor));
			while (it != promotions.end())
			{
				if (it->first >= limit) break;
				if (it->second.m_bHasPromotion) return it->first;
				if (promotions.size() > 32 && it->second.Empty()) promotions.erase(it++);
				else ++it;
			}
		}
		return -1;
	}

	// XML-only candidate lists. Preserve descending promotion-ID order and
	// collapse duplicate class entries; eligibility is still checked live.
	template<class Globals, class Promotion>
	void cacheFreeUnitCombat(const Globals& globals, std::vector<std::vector<Promotion> >& byCombat)
	{
		byCombat.assign(globals.getNumUnitCombatInfos(), std::vector<Promotion>());
		for (int id = globals.getNumPromotionInfos() - 1; id >= 0; --id)
		{
			const Promotion promotion = static_cast<Promotion>(id);
			for (int i = 0; i < globals.getPromotionInfo(promotion).getNumFreetoUnitCombats(); ++i)
			{
				const int combat = globals.getPromotionInfo(promotion).getFreetoUnitCombat(i);
				if (combat < 0 || combat >= (int)byCombat.size()) continue;
				std::vector<Promotion>& list = byCombat[combat];
				if (list.empty() || list.back() != promotion) list.push_back(promotion);
			}
		}
	}
}
