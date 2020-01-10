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
	DllExport CvCity* _findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL);
	CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const;	// Exposed to Python
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const;				// Exposed to Python
	DllExport bool isPlot(int iX, int iY) const;																		// Exposed to Python
	DllExport int numPlots() const; 																								// Exposed to Python
	int plotNum(int iX, int iY) const;																		// Exposed to Python
	int pointXToPlotX(float fX) const;
	DllExport float _plotXToPointX(int iX);
	float plotXToPointX(int iX) const;
	int pointYToPlotY(float fY) const;
	DllExport float _plotYToPointY(int iY);
	float plotYToPointY(int iY) const;
	DllExport int getGridWidth() const;																		// Exposed to Python
	DllExport int getGridHeight() const;																	// Exposed to Python
	int getLandPlots() const;																					// Exposed to Python
	int getOwnedPlots() const;																				// Exposed to Python
	DllExport bool _isWrapX();
	bool isWrapX() const;																							// Exposed to Python
	DllExport bool _isWrapY();
	bool isWrapY() const;																							// Exposed to Python
	DllExport bool _isWrap();
	bool isWrap() const;
	DllExport WorldSizeTypes _getWorldSize();
	WorldSizeTypes getWorldSize() const;															// Exposed to Python
	ClimateTypes getClimate() const;																	// Exposed to Python
	SeaLevelTypes getSeaLevel() const;																// Exposed to Python

	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;				// Exposed to Python
	DllExport CvPlot* plotByIndex(int iIndex) const;											// Exposed to Python
	DllExport CvPlot* plot(int iX, int iY) const;													// Exposed to Python
	DllExport CvPlot* _pointToPlot(float fX, float fY);
	CvPlot* pointToPlot(float fX, float fY) const;
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
