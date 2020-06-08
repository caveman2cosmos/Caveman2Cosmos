#pragma once

#ifndef CIV4_MAP_INTERFACE_BASE_H
#define CIV4_MAP_INTERFACE_BASE_H

//
//	FILE:	 CvMapInterfaceBase.h
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvArea.h"
#include "CvPlot.h"


//
// holds initialization info
//
struct CvMapInitData
{
	int m_iGridW;						// in game plots
	int m_iGridH;						// in game plots
	int m_iTopLatitude;
	int m_iBottomLatitude;

	bool m_bWrapX;
	bool m_bWrapY;

	CvMapInitData(int iGridW=0, int iGridH=0, int iTopLatitude=90, int iBottomLatitude=-90, bool bWrapX=false, bool bWrapY=false) :
		m_iGridH(iGridH),m_iGridW(iGridW),m_iTopLatitude(iTopLatitude),m_iBottomLatitude(iBottomLatitude),m_bWrapY(bWrapY),m_bWrapX(bWrapX)
	{ }
};


//
// CvMapInterfaceBase
//
class CvMapExternal;
class CvMapInterfaceBase
{
public:
	CvMapInterfaceBase();
	virtual ~CvMapInterfaceBase();

	CvMapExternal* getProxy();

	virtual CvMapInterfaceBase*	getUnderlyingMap() const = 0;

	virtual void init(CvMapInitData* pInitData=NULL) = 0;
	virtual void setupGraphical() = 0;
	virtual void reset(CvMapInitData* pInitData) = 0;

public:
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	virtual MapTypes getType() const = 0;
	virtual void setType(MapTypes eNewType) = 0;

	virtual void beforeSwitch() = 0;
	virtual void afterSwitch() = 0;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	virtual void erasePlots() = 0;																			// Exposed to Python
	virtual void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false) = 0;		// Exposed to Python
	virtual void setAllPlotTypes(PlotTypes ePlotType) = 0;												// Exposed to Python

	virtual void doTurn() = 0;																			

	virtual void updateFlagSymbols() = 0;

	virtual void updateFog() = 0;
	virtual void updateVisibility() = 0;																// Exposed to Python
	virtual void updateSymbolVisibility() = 0;
	virtual void updateSymbols() = 0;
	virtual void updateMinimapColor() = 0;															// Exposed to Python
	virtual void updateSight(bool bIncrement, bool bUpdatePlotGroups = true) = 0;
	virtual void updateIrrigated() = 0;
	virtual void updateCenterUnit() = 0;
	virtual void updateWorkingCity() = 0;
	virtual void updateMinOriginalStartDist(const CvArea* pArea) = 0;										// Exposed to Python
	virtual void updateYield() = 0;

	virtual void verifyUnitValidPlot() = 0;

	virtual CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100) = 0;// Exposed to Python 

	virtual CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const = 0;	// Exposed to Python
	virtual CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const = 0;				// Exposed to Python

	virtual CvArea* findBiggestArea(bool bWater) const = 0;																						// Exposed to Python

	virtual int getMapFractalFlags() const = 0;																												// Exposed to Python
	virtual bool findWater(const CvPlot* pPlot, int iRange, bool bFreshWater) const = 0;										// Exposed to Python

	virtual bool isPlot(int iX, int iY) const = 0;																		// Exposed to Python
	virtual int numPlots() const = 0; 																								// Exposed to Python

	virtual int plotNum(int iX, int iY) const = 0;																		// Exposed to Python
	virtual int plotX(int iIndex) const = 0;																										// Exposed to Python
	virtual int plotY(int iIndex) const = 0;																										// Exposed to Python

	virtual int pointXToPlotX(float fX) const = 0;
	virtual float plotXToPointX(int iX) const = 0;

	virtual int pointYToPlotY(float fY) const = 0;
	virtual float plotYToPointY(int iY) const = 0;

	virtual float getWidthCoords() const = 0;
	virtual float getHeightCoords() const = 0;

	virtual int maxPlotDistance() const = 0;																								// Exposed to Python
	virtual int maxStepDistance() const = 0;																								// Exposed to Python

	virtual int getGridWidth() const = 0;																		// Exposed to Python
	virtual int getGridHeight() const = 0;																	// Exposed to Python
	virtual int getLandPlots() const = 0;																					// Exposed to Python

	virtual int getOwnedPlots() const = 0;																				// Exposed to Python

	virtual int getTopLatitude() const = 0;																									// Exposed to Python
	virtual int getBottomLatitude() const = 0;																							// Exposed to Python

	virtual bool isWrapX() const = 0;																							// Exposed to Python
	virtual bool isWrapY() const = 0;																							// Exposed to Python
	virtual bool isWrap() const = 0;
	virtual WorldSizeTypes getWorldSize() const = 0;															// Exposed to Python
	virtual ClimateTypes getClimate() const = 0;																	// Exposed to Python
	virtual SeaLevelTypes getSeaLevel() const = 0;																// Exposed to Python

	virtual int getNumCustomMapOptions() const = 0;
	virtual CustomMapOptionTypes getCustomMapOption(int iOption) const = 0;				// Exposed to Python

	virtual CvPlot* plotByIndex(int iIndex) const = 0;											// Exposed to Python
	virtual CvPlot* plot(int iX, int iY) const = 0;													// Exposed to Python
	virtual CvPlot* pointToPlot(float fX, float fY) const = 0;

	virtual int getNumAreas() const = 0;														// Exposed to Python
	virtual int getNumLandAreas() const = 0;

	// Serialization:
	virtual void read(FDataStreamBase* pStream) = 0;
	virtual void write(FDataStreamBase* pStream) = 0;

private:
	CvMapExternal*	m_proxy;
};

#endif
