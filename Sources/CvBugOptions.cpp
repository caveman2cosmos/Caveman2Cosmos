/**********************************************************************

File:		CvBugOptions.cpp
Author:		EmperorFool
Created:	2009-01-21

		Copyright (c) 2009 The BUG Mod. All rights reserved.

**********************************************************************/

#include "CvGameCoreDLL.h"
#include "CvBugOptions.h"
#include "CvGlobals.h"
#include "CvPython.h"

bool g_bIsBug = false;

void setIsBug()
{
	logging::logMsg("bull.log", "isBug: true");
	g_bIsBug = true;
}


// BUG option values only ever change via the (rarely used) in-game options screen, but
// getBugOptionBOOL()/getBugOptionINT() are called from per-unit/per-city hot paths (eg.
// canAcquirePromotion, isForcedCityCycle). Without caching, every single call pays for a full
// Python round-trip - on a large map with many units/cities for one player that can add up to
// minutes of real time spent entirely in Python call overhead, which looks indistinguishable
// from the game being frozen. Cache each id's value after the first lookup instead. Trade-off:
// a BUG option changed mid-session via the options screen won't take effect until the game is
// restarted - far better than the alternative.
static std::map<CvString, bool> g_bugOptionBoolCache;
static std::map<CvString, int> g_bugOptionIntCache;

bool getBugOptionBOOL(const char* id, bool bDefault, const char* xmlKey)
{
	PROFILE_FUNC();

	if (g_bIsBug)
	{
		std::map<CvString, bool>::const_iterator itr = g_bugOptionBoolCache.find(id);
		if (itr != g_bugOptionBoolCache.end())
		{
			return itr->second;
		}
		const bool bResult = Cy::call<bool>(PYBugOptionsModule, "getOptionBOOL", Cy::Args(id, bDefault));
		g_bugOptionBoolCache[id] = bResult;
		return bResult;
	}
	if (!xmlKey)
	{
		CvString tmp;
		tmp.append(OPTION_XML_PREFIX);
		tmp.append(id);
		xmlKey = tmp.c_str();
	}
	//logMsg("debug - getBugOptionBOOL %s", xmlKey);
	return GC.getDefineBOOL(xmlKey, bDefault);
}

int getBugOptionINT(const char* id, int iDefault, const char* xmlKey)
{
	PROFILE_FUNC();

	if (g_bIsBug)
	{
		std::map<CvString, int>::const_iterator itr = g_bugOptionIntCache.find(id);
		if (itr != g_bugOptionIntCache.end())
		{
			return itr->second;
		}
		const int iResult = Cy::call<int>(PYBugOptionsModule, "getOptionINT", Cy::Args(id, iDefault));
		g_bugOptionIntCache[id] = iResult;
		return iResult;
	}
	if (!xmlKey)
	{
		CvString tmp;
		tmp.append(OPTION_XML_PREFIX);
		tmp.append(id);
		xmlKey = tmp.c_str();
	}
	//logMsg("debug - getBugOptionINT %s", xmlKey);
	return GC.getDefineINT(xmlKey, iDefault);
}
