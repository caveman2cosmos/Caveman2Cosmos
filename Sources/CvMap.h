#pragma once

#ifndef CIV4_MAP_H
#define CIV4_MAP_H

//
//	FILE:	 CvMap.h
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvPathGenerator.h"
#include "CvMapInterfaceBase.h"


class CvPlotGroup;


inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap && iRange != 0)
	{
		if (iCoord < 0 )
		{
			return (iRange + (iCoord % iRange));
		}
		else if (iCoord >= iRange)
		{
			return (iCoord % iRange);
		}
	}

	return iCoord;
}


//
// CvMap
//
class CvSelectionGroup;
class CvMap : public CvMapInterfaceBase
{

	friend class CyMap;

public:

	CvMap();
	explicit CvMap(/* Parallel Maps */ MapTypes eMap);
	virtual ~CvMap();

	CvMapInterfaceBase*	getUnderlyingMap() const { return const_cast<CvMap*>(this); }

	void init(CvMapInitData* pInitData=NULL);
	void setupGraphical();
	void reset(CvMapInitData* pInitData);

	void uninit();
protected:

	void setup();

public:
	//int percentUnoccupiedLand(bool bExcludeWater = true, bool bIncludeBarbarian = false, bool bExcludePeaks = true, CvArea* pArea = NULL, int iRange = -1, CvPlot* pRangeFromPlot = NULL);

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	MapTypes getType() const;
	void setType(MapTypes eNewType);

	void beforeSwitch();
	void afterSwitch();

	//	Viewports are owned by their underlying maps
	int	getNumViewports() const;
	CvViewport* getViewport(int iIndex) const;
	int addViewport(int iXOffset, int iYOffset, bool bIsFullMapContext);	//	Returns new viewport index
	void deleteViewport(int iIndex);
	void setCurrentViewport(int iIndex);
	CvViewport* getCurrentViewport() const;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	void erasePlots();																			// Exposed to Python
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false);		// Exposed to Python
	void resetRevealedPlots(TeamTypes eTeam);
	void setAllPlotTypes(PlotTypes ePlotType);												// Exposed to Python

	void doTurn();																			

	void updateFlagSymbolsInternal(bool bForce);
	void updateFlagSymbols();

	void updateFog();
	void updateVisibility();																// Exposed to Python
	void updateSymbolVisibility();
	void updateSymbols();
	void updateMinimapColor();															// Exposed to Python
	void updateSight(bool bIncrement, bool bUpdatePlotGroups = true);
	void updateIrrigated();
	void updateCenterUnit();
	void updateWorkingCity();
	void updateMinOriginalStartDist(const CvArea* pArea);										// Exposed to Python
	void updateYield();

	void verifyUnitValidPlot();

	void combinePlotGroups(PlayerTypes ePlayer, CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses);	

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100); // Exposed to Python 

	CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const;	// Exposed to Python
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const;				// Exposed to Python
	CvSelectionGroup* findSelectionGroupInternal(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers, bool bAllowViewportSwitch) const;				// Exposed to Python

	CvArea* findBiggestArea(bool bWater) const;																						// Exposed to Python

	int getMapFractalFlags() const;																												// Exposed to Python
	bool findWater(const CvPlot* pPlot, int iRange, bool bFreshWater) const;										// Exposed to Python

#ifdef _USRDLL
	inline bool isPlot(int iX, int iY) const // Exposed to Python
	{
		return (iX >= 0 && iX < getGridWidth() && iY >= 0 && iY < getGridHeight());
	}
#endif

	inline int numPlots() const { return getGridWidth() * getGridHeight(); } // Exposed to Python

	inline int plotNum(int iX, int iY) const { return (iY * getGridWidth()) + iX; } // Exposed to Python

	int plotX(int iIndex) const;																										// Exposed to Python
	int plotY(int iIndex) const;																										// Exposed to Python

	int pointXToPlotX(float fX) const;
	float plotXToPointX(int iX) const;

	int pointYToPlotY(float fY) const;
	float plotYToPointY(int iY) const;

	float getWidthCoords() const;
	float getHeightCoords() const;

	int maxPlotDistance() const;																								// Exposed to Python
	int maxStepDistance() const;																								// Exposed to Python

	inline int getGridWidth() const { return m_iGridWidth; } // Exposed to Python
	inline int getGridHeight() const { return m_iGridHeight; } // Exposed to Python

	int getLandPlots() const;																					// Exposed to Python
	void changeLandPlots(int iChange);

	int getOwnedPlots() const;																				// Exposed to Python
	void changeOwnedPlots(int iChange);

	int getTopLatitude() const;																									// Exposed to Python
	int getBottomLatitude() const;																							// Exposed to Python

	int getNextRiverID() const;																									// Exposed to Python
	void incrementNextRiverID();																					// Exposed to Python

	inline bool isWrapX() const { return m_bWrapX; } // Exposed to Python
	inline bool isWrapY() const { return m_bWrapY; } // Exposed to Python
	inline bool isWrap() const { return m_bWrapX || m_bWrapY; } // Exposed to Python

	WorldSizeTypes getWorldSize() const;															// Exposed to Python
	ClimateTypes getClimate() const;																	// Exposed to Python
	SeaLevelTypes getSeaLevel() const;																// Exposed to Python

	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;				// Exposed to Python

	int getNumBonuses(BonusTypes eIndex) const;																	// Exposed to Python
	void changeNumBonuses(BonusTypes eIndex, int iChange);

	int getNumBonusesOnLand(BonusTypes eIndex) const;														// Exposed to Python
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);

	inline CvPlot* plotByIndex(int iIndex) const // Exposed to Python
	{
		return (iIndex >= 0 && iIndex < getGridWidth() * getGridHeight()) ? &(m_pMapPlots[iIndex]) : NULL;
	}

	__forceinline CvPlot* plot(int iX, int iY) const // Exposed to Python
	{
		if (iX == INVALID_PLOT_COORD || iY == INVALID_PLOT_COORD)
		{
			return NULL;
		}
		const int iMapX = coordRange(iX, getGridWidth(), isWrapX());
		const int iMapY = coordRange(iY, getGridHeight(), isWrapY());
		return isPlot(iMapX, iMapY) ? &(m_pMapPlots[plotNum(iMapX, iMapY)]) : NULL;
	}

	__forceinline CvPlot* plotSorenINLINE(int iX, int iY) const
	{
		if (iX == INVALID_PLOT_COORD || iY == INVALID_PLOT_COORD)
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNum(iX, iY)]);
	}

	CvPlot* pointToPlot(float fX, float fY) const;

	int getIndexAfterLastArea() const;														// Exposed to Python
	int getNumAreas() const;														// Exposed to Python
	int getNumLandAreas() const;
	CvArea* getArea(int iID) const;																// Exposed to Python
	CvArea* addArea();
	void deleteArea(int iID);

	// city iteration
	DECLARE_INDEX_ITERATOR(const CvMap, CvArea, area_iterator, firstArea, nextArea);
	area_iterator beginAreas() const { return area_iterator(this); }
	area_iterator endAreas() const { return area_iterator(); }
	typedef bst::iterator_range<area_iterator> area_range;
	area_range areas() const { return area_range(beginAreas(), endAreas()); }

	// deprecated, use area_range	
	CvArea* firstArea(int *pIterIdx, bool bRev=false) const; // Exposed to Python
	// deprecated, use area_range	
	CvArea* nextArea(int *pIterIdx, bool bRev=false) const; // Exposed to Python

	void recalculateAreas();																		// Exposed to Python

	void resetPathDistance();																		// Exposed to Python
	// Super Forts begin *canal* *choke*
	int calculatePathDistance(const CvPlot* pSource, const CvPlot* pDest, const CvPlot* pInvalidPlot = NULL) const;	// Exposed to Python
	void calculateCanalAndChokePoints();	// Exposed to Python
	// Super Forts end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	void invalidateIsActivePlayerNoDangerCache();
	void invalidateIsTeamBorderCache(TeamTypes eTeam);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	// Serialization:
	 virtual void read(FDataStreamBase* pStream);
	 virtual void write(FDataStreamBase* pStream);

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * eCustomMapOptions);		// Exposed to Python

	void toggleUnitsDisplay();
	void toggleCitiesDisplay();

protected:

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNextRiverID;

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	MapTypes m_eType;
	std::vector<CvViewport*> m_viewports;
	int m_iCurrentViewportIndex;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	bool m_bWrapX;
	bool m_bWrapY;

	int* m_paiNumBonus;
	int* m_paiNumBonusOnLand;

	bool m_bCitiesDisplayed;
	bool m_bUnitsDisplayed;

	CvPlot* m_pMapPlots;

	FFreeListTrashArray<CvArea> m_areas;

	void calculateAreas();
};

#endif
