#pragma once

#ifndef CIV4_MAP_EXTERNAL_H
#define CIV4_MAP_EXTERNAL_H

//
//	FILE:	 CvMapExternal.h
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//


#include "CvArea.h"
#include "CvPlot.h"
#include "CvMapInterfaceBase.h"


class FAStar;
class CvPlotGroup;


//
// CvMap
//
class CvSelectionGroup;
class CvMapExternal
{

	friend class CyMap;

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
	DllExport CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);	// Exposed to Python
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false);				// Exposed to Python
	DllExport bool isPlot(int iX, int iY) const;																		// Exposed to Python
	DllExport int numPlots() const; 																								// Exposed to Python
	int plotNum(int iX, int iY) const;																		// Exposed to Python
	int pointXToPlotX(float fX);
	DllExport float plotXToPointX(int iX);
	int pointYToPlotY(float fY);
	DllExport float plotYToPointY(int iY);
	DllExport int getGridWidth() const;																		// Exposed to Python
	DllExport int getGridHeight() const;																	// Exposed to Python
	int getLandPlots();																					// Exposed to Python
	int getOwnedPlots();																				// Exposed to Python
	DllExport bool isWrapX();																							// Exposed to Python
	DllExport bool isWrapY();																							// Exposed to Python
	DllExport bool isWrap();
	DllExport WorldSizeTypes getWorldSize();															// Exposed to Python
	ClimateTypes getClimate();																	// Exposed to Python
	SeaLevelTypes getSeaLevel();																// Exposed to Python

	int getNumCustomMapOptions();
	CustomMapOptionTypes getCustomMapOption(int iOption);				// Exposed to Python
	DllExport CvPlot* plotByIndex(int iIndex) const;											// Exposed to Python
	DllExport CvPlot* plot(int iX, int iY) const;													// Exposed to Python
	DllExport CvPlot* pointToPlot(float fX, float fY);
	int getNumAreas();														// Exposed to Python
	int getNumLandAreas();
	// Serialization:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

private:
	CvMapInterfaceBase*	m_proxiedMap;
	mutable std::map<int,CvPlot*>		m_psuedoPlots;
	bool m_bMapCoordinates;
};

#endif
