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

#include "CvArea.h"
#include "CvMapInterfaceBase.h"
#include "CvPlot.h"
#include "index_iterator_base.h"

class CvCity;
class CvPlotGroup;
class CvSelectionGroup;
class CvViewport;
struct TravelingUnit;

inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap && iRange != 0)
	{
		if (iCoord < 0 )
		{
			return iRange + (iCoord % iRange);
		}
		if (iCoord >= iRange)
		{
			return iCoord % iRange;
		}
	}
	return iCoord;
}


//
// CvMap
//
class CvMap : public CvMapInterfaceBase
{
	friend class CyMap;

public:
	explicit CvMap(MapTypes eMap);
	virtual ~CvMap();

	CvMapInterfaceBase*	getUnderlyingMap() const { return const_cast<CvMap*>(this); }

	void init(CvMapInitData* pInitData=NULL);
	void setupGraphical();
	void reset(CvMapInitData* pInitData);

protected:
	void uninit();
	void setup();
	void addViewport(int iXOffset, int iYOffset);

public:
	MapTypes getType() const;

	void beforeSwitch();
	void afterSwitch();

	void updateIncomingUnits();
	void moveUnitToMap(CvUnit& unit, int numTravelTurns);
	void deleteOffMapUnits();

	//void deleteViewport(int iIndex);
	CvViewport* getCurrentViewport() const;
	const std::vector<CvViewport*>& getViewports() const;

	const char* getMapScript() const;

	bool plotsInitialized() const;

	void erasePlots();
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false);
	void resetRevealedPlots(TeamTypes eTeam);
	void setAllPlotTypes(PlotTypes ePlotType);

	void doTurn();

	void updateFlagSymbolsInternal(bool bForce);
	void updateFlagSymbols();
	void updateFog(const bool bApplyDecay = false);
	void InitFogDecay(const bool bWithRandom = false);
	void updateVisibility();
	void updateSymbolVisibility();
	void updateSymbols();
	void updateMinimapColor();
	void updateSight(bool bIncrement, bool bUpdatePlotGroups = true);
	void updateIrrigated();
	void updateCenterUnit();
	void updateWorkingCity();
	void updateMinOriginalStartDist(const CvArea* pArea);
	void updateYield();

	void verifyUnitValidPlot();

	static void combinePlotGroups(CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses);

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100);

	CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const;
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false) const;
	CvSelectionGroup* findSelectionGroupInternal(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers, bool bAllowViewportSwitch) const;

	CvArea* findBiggestArea(bool bWater) const;

	int getMapFractalFlags() const;
	bool findWater(const CvPlot* pPlot, int iRange, bool bFreshWater) const;

	inline bool isPlot(int iX, int iY) const
	{
		return (iX >= 0 && iX < getGridWidth() && iY >= 0 && iY < getGridHeight());
	}

	inline int numPlots() const { return getGridWidth() * getGridHeight(); }

	inline int plotNum(int iX, int iY) const { return (iY * getGridWidth()) + iX; }

	int plotX(int iIndex) const;
	int plotY(int iIndex) const;

	int pointXToPlotX(float fX) const;
	float plotXToPointX(int iX) const;

	int pointYToPlotY(float fY) const;
	float plotYToPointY(int iY) const;

	float getWidthCoords() const;
	float getHeightCoords() const;

	int maxPlotDistance() const;
	int maxStepDistance() const;

	inline int getGridWidth() const { return m_iGridWidth; }
	inline int getGridHeight() const { return m_iGridHeight; }

	int getLandPlots() const;
	void changeLandPlots(int iChange);

	int getOwnedPlots() const;
	void changeOwnedPlots(int iChange);

	int getTopLatitude() const;
	int getBottomLatitude() const;

	int getNextRiverID() const;
	void incrementNextRiverID();

	inline bool isWrapX() const { return m_bWrapX; }
	inline bool isWrapY() const { return m_bWrapY; }
	inline bool isWrap() const { return m_bWrapX || m_bWrapY; }

	WorldSizeTypes getWorldSize() const;
	ClimateTypes getClimate() const;
	SeaLevelTypes getSeaLevel() const;

	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;

	int getNumBonuses(BonusTypes eIndex) const;
	void changeNumBonuses(BonusTypes eIndex, int iChange);

	int getNumBonusesOnLand(BonusTypes eIndex) const;
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);

	inline CvPlot* plotByIndex(int iIndex) const
	{
		return (iIndex >= 0 && iIndex < getGridWidth() * getGridHeight()) ? &(m_pMapPlots[iIndex]) : NULL;
	}

	__forceinline CvPlot* plot(int iX, int iY) const
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

	int getIndexAfterLastArea() const;
	int getNumAreas() const;
	int getNumLandAreas() const;
	CvArea* getArea(int iID) const;
	CvArea* addArea();
	void deleteArea(int iID);

	// city iteration
	DECLARE_INDEX_ITERATOR(const CvMap, CvArea, area_iterator, firstArea, nextArea);
	area_iterator beginAreas() const { return area_iterator(this); }
	area_iterator endAreas() const { return area_iterator(); }
	typedef bst::iterator_range<area_iterator> area_range;
	area_range areas() const { return area_range(beginAreas(), endAreas()); }

	// deprecated, use area_range
	CvArea* firstArea(int *pIterIdx, bool bRev=false) const;
	// deprecated, use area_range
	CvArea* nextArea(int *pIterIdx, bool bRev=false) const;

	void recalculateAreas();
	void resetPathDistance();

	// Super Forts *canal* *choke*
	int calculatePathDistance(const CvPlot* pSource, const CvPlot* pDest, const CvPlot* pInvalidPlot = NULL) const;
	void calculateCanalAndChokePoints();

	// Plot danger cache
	void invalidateActivePlayerPlotCache();
	void invalidateBorderDangerCache(TeamTypes eTeam);

	const std::pair<CvPlot*, CvPlot*> plots() const;

	// Serialization:
	 virtual void read(FDataStreamBase* pStream);
	 virtual void write(FDataStreamBase* pStream);

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * eCustomMapOptions);

	void toggleUnitsDisplay();
	void toggleCitiesDisplay();

	void setClimateZone(const int y, const ClimateZoneTypes eClimateZone);
	ClimateZoneTypes getClimateZone(const int y);

private:
	void calculateAreas();

	const MapTypes m_eType;

	int m_iGridWidth;
	int m_iGridHeight;
	int m_iLandPlots;
	int m_iOwnedPlots;
	int m_iTopLatitude;
	int m_iBottomLatitude;
	int m_iNextRiverID;
	int m_iCurrentViewportIndex;

	bool m_bWrapX;
	bool m_bWrapY;

	bool m_bCitiesDisplayed;
	bool m_bUnitsDisplayed;

	static bool m_bSwitchInProgress;

	int* m_paiNumBonus;
	int* m_paiNumBonusOnLand;

	CvPlot* m_pMapPlots;

	FFreeListTrashArray<CvArea> m_areas;

	std::vector<CvViewport*> m_viewports;

	std::vector<TravelingUnit*> m_IncomingUnits;
	ClimateZoneTypes* m_climateZones;
};

#endif
