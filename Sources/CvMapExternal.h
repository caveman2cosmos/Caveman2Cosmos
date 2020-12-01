#pragma once

#ifndef CIV4_MAP_EXTERNAL_H
#define CIV4_MAP_EXTERNAL_H

//
//	FILE:	 CvMapExternal.h
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//

#include "CvMapInterfaceBase.h"

//
// CvMap
//
class CvCity;
class CvSelectionGroup;
class CvMapExternal
{
public:
	CvMapExternal(CvMapInterfaceBase* proxiedMap);

	void mapCoordinates(bool bNewValue) { m_bMapCoordinates = bNewValue; }

	CvMapExternal();
	virtual ~CvMapExternal();

	DllExport void init(CvMapInitData* pInitData=NULL);
	DllExport void setupGraphical();
	DllExport void reset(CvMapInitData* pInitData);

	DllExport void erasePlots();
	DllExport void updateFlagSymbols();

	DllExport void updateFog();
	void updateVisibility();
	DllExport void updateSymbolVisibility();
	DllExport void updateMinimapColor();
	DllExport void updateCenterUnit();
	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const;
	DllExport bool isPlot(int iX, int iY) const;
	DllExport int numPlots() const;
	int plotNum(int iX, int iY) const;
	int pointXToPlotX(float fX) const;
	DllExport float plotXToPointX(int iX);
	int pointYToPlotY(float fY) const;
	DllExport float plotYToPointY(int iY);
	DllExport int getGridWidth() const;
	DllExport int getGridHeight() const;
	int getLandPlots() const;
	int getOwnedPlots() const;
	DllExport bool isWrapX();
	DllExport bool isWrapY();
	DllExport bool isWrap();
	DllExport WorldSizeTypes getWorldSize();
	ClimateTypes getClimate() const;
	SeaLevelTypes getSeaLevel() const;

	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;
	DllExport CvPlot* plotByIndex(int iIndex) const;
	DllExport CvPlot* plot(int iX, int iY) const;
	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getNumAreas() const;
	int getNumLandAreas() const;
	// Serialization:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

private:
	CvMapInterfaceBase*	m_proxiedMap;
	mutable std::map<int,CvPlot*>		m_psuedoPlots;
	bool m_bMapCoordinates;
};

#endif
