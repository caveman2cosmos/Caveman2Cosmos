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

	DllExport void erasePlots();																			// Exposed to Python
	DllExport void updateFlagSymbols();

	DllExport void updateFog();
	void updateVisibility();																// Exposed to Python
	DllExport void updateSymbolVisibility();
	DllExport void updateMinimapColor();															// Exposed to Python
	DllExport void updateCenterUnit();
	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const;				// Exposed to Python
	DllExport bool isPlot(int iX, int iY) const;																		// Exposed to Python
	DllExport int numPlots() const; 																								// Exposed to Python
	int plotNum(int iX, int iY) const;																		// Exposed to Python
	int pointXToPlotX(float fX) const;
	DllExport float plotXToPointX(int iX);
	int pointYToPlotY(float fY) const;
	DllExport float plotYToPointY(int iY);
	DllExport int getGridWidth() const;																		// Exposed to Python
	DllExport int getGridHeight() const;																	// Exposed to Python
	int getLandPlots() const;																					// Exposed to Python
	int getOwnedPlots() const;																				// Exposed to Python
	DllExport bool isWrapX();
	DllExport bool isWrapY();
	DllExport bool isWrap();
	DllExport WorldSizeTypes getWorldSize();
	ClimateTypes getClimate() const;																	// Exposed to Python
	SeaLevelTypes getSeaLevel() const;																// Exposed to Python

	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;				// Exposed to Python
	DllExport CvPlot* plotByIndex(int iIndex) const;											// Exposed to Python
	DllExport CvPlot* plot(int iX, int iY) const;													// Exposed to Python
	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getNumAreas() const;														// Exposed to Python
	int getNumLandAreas()const;
	// Serialization:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

private:
	CvMapInterfaceBase*	m_proxiedMap;
	mutable std::map<int,CvPlot*>		m_psuedoPlots;
	bool m_bMapCoordinates;
};

#endif
