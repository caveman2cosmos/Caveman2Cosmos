/* Toffer - CounterSet.cpp
 Simple tool when a small set of counters is needed.
 Set is not dynamically sized, and it is not optimal for large amount of counters.
*/

#include "FProfiler.h"

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

void CounterSet::init(const int iNumCounters)
{
	PROFILE_EXTRA_FUNC();
	iSize = safeCoercion(iNumCounters);
	iNext = 0;
	iArrCounters = new uint16_t[iSize];
	iArrCurrentIDs = new int[iSize];

	for (int iI = 0; iI < iSize; iI++)
	{
		iArrCounters[iI] = 0;
		iArrCurrentIDs[iI] = 0;
	}
}

uint16_t CounterSet::getCount(const int ID)
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < iSize; iI++)
	{
		if (iArrCurrentIDs[iI] == ID)
		{
			iArrCounters[iI] = safeCoercion(iValue);
			return;
		}
	}
	assign(ID, safeCoercion(iValue));
}

void CounterSet::assign(const int ID, const uint16_t iValue)
{
	iArrCounters[iNext] = iValue;
	iArrCurrentIDs[iNext] = ID;
	if (++iNext == iSize) iNext = 0;
}

uint16_t CounterSet::safeCoercion(const int iValue)
{
	if (iValue > 0 || iValue <= 65535)
	{
		return static_cast<uint16_t>(iValue);
	}
	return 0;
}