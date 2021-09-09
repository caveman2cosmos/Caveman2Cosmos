/**********************************************************************

File:		CvBugOptions.cpp
Author:		EmperorFool
Created:	2009-01-21

		Copyright (c) 2009 The BUG Mod. All rights reserved.

**********************************************************************/

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvPython.h"

bool g_bIsBug = false;

void setIsBug()
{
	logging::logMsg("bull.log", "isBug: true");
	g_bIsBug = true;

	// set the unit and building filters to default state once Bug is available
	UnitFilterList::setFilterActiveAll(UNIT_FILTER_HIDE_UNBUILDABLE, getBugOptionBOOL("CityScreen__HideUntrainableUnits", false));
	BuildingFilterList::setFilterActiveAll(BUILDING_FILTER_HIDE_UNBUILDABLE, getBugOptionBOOL("CityScreen__HideUnconstructableBuildings", false));
}


bool getBugOptionBOOL(const char* id, bool bDefault, const char* xmlKey)
{
	PROFILE_FUNC();

	if (g_bIsBug)
	{
		return Cy::call<bool>(PYBugOptionsModule, "getOptionBOOL", Cy::Args(id, bDefault));
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
		return Cy::call<int>(PYBugOptionsModule, "getOptionINT", Cy::Args(id, iDefault));
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
