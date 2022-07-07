#pragma once
// CityOutputHistory.h
#ifndef CITY_OUTPUT_HISTORY_H
#define CITY_OUTPUT_HISTORY_H

#include "CvEnums.h"

class CityOutputHistory
{
	bool notInRange(const uint16_t iHistory) const { return iHistory < 0 || iHistory >= iHistorySize; }
	static uint16_t iHistorySize;
	uint32_t* recentOutputTurn;
	std::vector< std::vector< std::pair<OrderTypes, uint16_t> > > recentOutputHistory;

  public:
	CityOutputHistory();
	~CityOutputHistory();
	void reset();

	static void setCityOutputHistorySize(const uint16_t iSize) { iHistorySize = iSize; }
	static uint16_t getCityOutputHistorySize() { return iHistorySize; }

	uint16_t getSize() { return getCityOutputHistorySize(); }
	void addToHistory(OrderTypes eOrder, uint16_t iType, short iHistory=-1);
	void setRecentOutputTurn(const uint16_t iHistory, const int iGameTurn) const;
	uint32_t getRecentOutputTurn(const uint16_t iHistory) const;
	uint16_t getCityOutputHistoryNumEntries(const uint16_t iHistory) const;
	uint16_t getCityOutputHistoryEntry(const uint16_t iHistory, const uint16_t iEntry, const bool bFirst) const;
};

#endif