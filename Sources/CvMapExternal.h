#pragma once

#ifndef CIV4_MAP_EXTERNAL_H
#define CIV4_MAP_EXTERNAL_H

//
//	FILE:	 CvMapExternal.h
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//

#include "CvMapInterfaceBase.h"

class CvCity;
class CvPlot;

class CvMapExternal
{
public:
	CvMapExternal(CvMapInterfaceBase* proxiedMap);
	virtual ~CvMapExternal();

	DllExport void init(CvMapInitData* pInitData=NULL);
	DllExport void setupGraphical();
	DllExport void reset(CvMapInitData* pInitData);

	DllExport void erasePlots();

	DllExport void updateFlagSymbols();
	DllExport void updateFog();
	DllExport void updateSymbolVisibility();
	DllExport void updateMinimapColor();
	DllExport void updateCenterUnit();

	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);

	DllExport bool isPlot(int iX, int iY) const;
	DllExport int numPlots() const;

	DllExport float plotXToPointX(int iX);
	DllExport float plotYToPointY(int iY);

	DllExport int getGridWidth() const;
	DllExport int getGridHeight() const;

	DllExport bool isWrapX();
	DllExport bool isWrapY();
	DllExport bool isWrap();

	DllExport WorldSizeTypes getWorldSize();

	DllExport CvPlot* plotByIndex(int iIndex) const;
	DllExport CvPlot* plot(int iX, int iY) const;
	DllExport CvPlot* pointToPlot(float fX, float fY);

	// Serialization:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

private:
	CvMapInterfaceBase*	m_proxiedMap;
};

#endif
