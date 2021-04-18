/**********************************************************************

File:		CvBugOptions.cpp
Author:		EmperorFool
Created:	2009-01-21

		Copyright (c) 2009 The BUG Mod. All rights reserved.

**********************************************************************/

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvPython.h"
#include "FVariableSystem.h"

bool g_bIsBug = false;

void setIsBug()
{
	logging::logMsg("bull.log", "isBug: true");
	g_bIsBug = true;

	// set the unit and building filters to default state once Bug is available
	UnitFilterList::setFilterActiveAll(UNIT_FILTER_HIDE_UNBUILDABLE, getBugOptionBOOL("CityScreen__HideUntrainableUnits", false));
	BuildingFilterList::setFilterActiveAll(BUILDING_FILTER_HIDE_UNBUILDABLE, getBugOptionBOOL("CityScreen__HideUnconstructableBuildings", false));
}


bool getDefineBOOL(const char* xmlKey, bool bDefault)
{
	int iResult = 0;
	if (GC.getDefinesVarSystem()->GetValue(xmlKey, iResult))
	{
		return iResult != 0;
	}
	return bDefault;
}

int getDefineINT(const char* xmlKey, int iDefault)
{
	int iResult = 0;
	if (GC.getDefinesVarSystem()->GetValue(xmlKey, iResult))
	{
		return iResult;
	}
	return iDefault;
}

float getDefineFLOAT(const char* xmlKey, float fDefault)
{
	float fResult = 0.0;
	if (GC.getDefinesVarSystem()->GetValue(xmlKey, fResult))
	{
		return fResult;
	}
	return fDefault;
}

const char * getDefineSTRING(const char* xmlKey, const char * szDefault)
{
	const char * szResult = NULL;
	if (GC.getDefinesVarSystem()->GetValue(xmlKey, szResult))
	{
		return szResult;
	}
	return szDefault;
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
	//logging::logMsg("bull.log", "debug - getBugOptionBOOL %s", xmlKey);
	return getDefineBOOL(xmlKey, bDefault);
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
	//logging::logMsg("bull.log", "debug - getBugOptionINT %s", xmlKey);
	return getDefineINT(xmlKey, iDefault);
}
