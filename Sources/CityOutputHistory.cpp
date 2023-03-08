/* Toffer - CityOutputHistory.cpp
 Essentially an extension on CvCity, i.e. not a generic class.
 Not far from being a generic though.
 */

#include "FProfiler.h"

#include "CityOutputHistory.h"
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"

CityOutputHistory::CityOutputHistory()
{
	recentOutputTurn = new uint32_t[iHistorySize]();
	recentOutputHistory.resize(iHistorySize);
}
CityOutputHistory::~CityOutputHistory()
{
	SAFE_DELETE_ARRAY(recentOutputTurn);
}

void CityOutputHistory::reset()
{
	PROFILE_EXTRA_FUNC();
	for (uint16_t iI = 0; iI < iHistorySize; iI++)
	{
		recentOutputTurn[iI] = 0;
	}
	recentOutputHistory.clear();
	recentOutputHistory.resize(iHistorySize);
}

void CityOutputHistory::addToHistory(OrderTypes eOrder, uint16_t iType, short iHistory)
{
	PROFILE_EXTRA_FUNC();
	if (iHistory > -1) // Used when reading saves.
	{
		if (iHistory < static_cast<int>(iHistorySize))
		{
			recentOutputHistory[iHistory].push_back(std::make_pair(eOrder, iType));
		}
		return;
	}
	const int iGameTurn = GC.getGame().getGameTurn();

	if (iGameTurn != recentOutputTurn[0])
	{
		// Iterate history
		for (uint16_t i = iHistorySize - 1; i > 0; i--)
		{
			recentOutputHistory[i] = recentOutputHistory[i-1];
			recentOutputTurn[i] = recentOutputTurn[i-1];
		}
		recentOutputHistory[0].clear();
		recentOutputTurn[0] = iGameTurn;
	}
	recentOutputHistory[0].push_back(std::make_pair(eOrder, iType));
}

// Used when reading saves.
void CityOutputHistory::setRecentOutputTurn(const uint16_t iHistory, const int iGameTurn) const
{
	if (notInRange(iHistory)) return;

	recentOutputTurn[iHistory] = iGameTurn;
}

uint32_t CityOutputHistory::getRecentOutputTurn(const uint16_t iHistory) const
{
	if (notInRange(iHistory)) return 0;

	return recentOutputTurn[iHistory];
}

uint16_t CityOutputHistory::getCityOutputHistoryNumEntries(const uint16_t iHistory) const
{
	if (notInRange(iHistory)) return 0;

	return recentOutputHistory[iHistory].size();
}

uint16_t CityOutputHistory::getCityOutputHistoryEntry(const uint16_t iHistory, const uint16_t iEntry, const bool bFirst) const
{
	if (notInRange(iHistory)) return NULL;

	if (bFirst)
	{
		return static_cast<uint16_t>(recentOutputHistory[iHistory][iEntry].first);
	}
	return recentOutputHistory[iHistory][iEntry].second;
}

uint16_t CityOutputHistory::iHistorySize;