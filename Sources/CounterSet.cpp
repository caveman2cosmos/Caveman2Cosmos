// Toffer - Simple tool when a limited amount of counters are needed.
// Counter set is not dynamically sized, not optimal for large amount of counters.
// If old counters gets less relevant to store for each new one added then a small set may be optimal anyway.

#include "CvGameCoreDLL.h"
#include "CounterSet.h"

CounterSet::CounterSet() { init(8); }

CounterSet::~CounterSet()
{
	SAFE_DELETE_ARRAY(iArrCounters);
	SAFE_DELETE_ARRAY(iArrCurrentIDs);
}

CounterSet::CounterSet(const int iNumCounters)
{
	init(iNumCounters);
}

void CounterSet::init(int iNumCounters)
{
	if (iNumCounters < 1) iNumCounters = 1;

	iNext = 0;
	iSize = iNumCounters;
	iArrCounters = new int[iNumCounters];
	iArrCurrentIDs = new int[iNumCounters];
	for (int iI = 0; iI < iNumCounters; iI++)
	{
		iArrCounters[iI] = 0;
		iArrCurrentIDs[iI] = 0;
	}
}

int CounterSet::getCount(const int ID)
{
	for (int iI = 0; iI < iSize; iI++)
	{
		if (iArrCurrentIDs[iI] == ID)
		{
			return iArrCounters[iI];
		}
	}
	assign(ID, 0);
	return 0;
}

void CounterSet::setCount(const int ID, const int iValue)
{
	for (int iI = 0; iI < iSize; iI++)
	{
		if (iArrCurrentIDs[iI] == ID)
		{
			iArrCounters[iI] = iValue;
			return;
		}
	}
	assign(ID, iValue);
}

void CounterSet::assign(const int ID, const int iValue)
{
	iArrCounters[iNext] = iValue;
	iArrCurrentIDs[iNext] = ID;
	if (++iNext == iSize) iNext = 0;
}