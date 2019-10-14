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
#include "CvPlot.h"
#include "CvPathGenerator.h"
#include "CvMapInterfaceBase.h"


class FAStar;
class CvPlotGroup;


inline int coordRange(int iCoord, int iRange, bool bWrap)
{
	if (bWrap)
	{
		if (iRange != 0)
		{
			if (iCoord < 0)
			{
				return (iRange + (iCoord % iRange));
			}
			else if (iCoord >= iRange)
			{
				return (iCoord % iRange);
			}
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

	CvMapInterfaceBase* getUnderlyingMap() const { return const_cast<CvMap*>(this); }

	void init(CvMapInitData* pInitData = NULL);
	void setupGraphical();
	void reset(CvMapInitData* pInitData);

	void uninit();

  protected:
	void setup();

  public:
	/************************************************************************************************/
	/* Afforess	                  Start		 07/27/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	int percentUnoccupiedLand(bool bExcludeWater = true, bool bIncludeBarbarian = false, bool bExcludePeaks = true, CvArea* pArea = NULL, int iRange = -1, CvPlot* pRangeFromPlot = NULL);
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
	/*********************************/
	/***** Parallel Maps - Begin *****/
	/*********************************/
	MapTypes getType() const;
	void	 setType(MapTypes eNewType);

	void beforeSwitch();
	void afterSwitch();

	//	Viewports are owned by their underlying maps
	int			getNumViewports() const;
	CvViewport* getViewport(int iIndex) const;
	int			addViewport(int iXOffset, int iYOffset, bool bIsFullMapContext); //	Returns new viewport index
	void		deleteViewport(int iIndex);
	void		setCurrentViewport(int iIndex);
	CvViewport* getCurrentViewport() const;
	/*******************************/
	/***** Parallel Maps - End *****/
	/*******************************/

	void erasePlots(); // Exposed to Python
	void setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly = false); // Exposed to Python
	void resetRevealedPlots(TeamTypes eTeam);
	void setAllPlotTypes(PlotTypes ePlotType); // Exposed to Python

	void doTurn();

	void updateFlagSymbolsInternal(bool bForce);
	void updateFlagSymbols();

	void updateFog();
	void updateVisibility(); // Exposed to Python
	void updateSymbolVisibility();
	void updateSymbols();
	void updateMinimapColor(); // Exposed to Python
	void updateSight(bool bIncrement, bool bUpdatePlotGroups = true);
	void updateIrrigated();
	void updateCenterUnit();
	void updateWorkingCity();
	void updateMinOriginalStartDist(CvArea* pArea); // Exposed to Python
	void updateYield();

	void verifyUnitValidPlot();

	void combinePlotGroups(PlayerTypes ePlayer, CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses);

	CvPlot* syncRandPlot(int iFlags = 0, int iArea = -1, int iMinUnitDistance = -1, int iTimeout = 100); // Exposed to Python

	CvCity*			  findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL); // Exposed to Python
	CvSelectionGroup* findSelectionGroup(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, bool bReadyToSelect = false, bool bWorkers = false); // Exposed to Python
	CvSelectionGroup* findSelectionGroupInternal(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers, bool bAllowViewportSwitch); // Exposed to Python

	CvArea* findBiggestArea(bool bWater); // Exposed to Python

	int	 getMapFractalFlags(); // Exposed to Python
	bool findWater(CvPlot* pPlot, int iRange, bool bFreshWater); // Exposed to Python

	bool isPlot(int iX, int iY) const; // Exposed to Python
#ifdef _USRDLL
	inline int isPlotINLINE(int iX, int iY) const
	{
		return ((iX >= 0) && (iX < getGridWidthINLINE()) && (iY >= 0) && (iY < getGridHeightINLINE()));
	}
#endif
	int numPlots() const; // Exposed to Python
#ifdef _USRDLL
	inline int numPlotsINLINE() const
	{
		return getGridWidthINLINE() * getGridHeightINLINE();
	}
#endif
	int plotNum(int iX, int iY) const; // Exposed to Python
#ifdef _USRDLL
	inline int plotNumINLINE(int iX, int iY) const
	{
		return ((iY * getGridWidthINLINE()) + iX);
	}
	inline int plotNumINLINE_checked(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return -1;
		}
		int iMapX = coordRange(iX, getGridWidthINLINE(), isWrapXINLINE());
		int iMapY = coordRange(iY, getGridHeightINLINE(), isWrapYINLINE());
		return (isPlotINLINE(iMapX, iMapY) ? plotNumINLINE(iMapX, iMapY) : -1);
	}
#endif
	int plotX(int iIndex) const; // Exposed to Python
	int plotY(int iIndex) const; // Exposed to Python

	int	  pointXToPlotX(float fX);
	float plotXToPointX(int iX);

	int	  pointYToPlotY(float fY);
	float plotYToPointY(int iY);

	float getWidthCoords();
	float getHeightCoords();

	int maxPlotDistance(); // Exposed to Python
	int maxStepDistance(); // Exposed to Python

	int getGridWidth() const; // Exposed to Python
#ifdef _USRDLL
	inline int getGridWidthINLINE() const
	{
		return m_iGridWidth;
	}
#endif
	int getGridHeight() const; // Exposed to Python
#ifdef _USRDLL
	inline int getGridHeightINLINE() const
	{
		return m_iGridHeight;
	}
#endif
	int	 getLandPlots(); // Exposed to Python
	void changeLandPlots(int iChange);

	int	 getOwnedPlots(); // Exposed to Python
	void changeOwnedPlots(int iChange);

	int getTopLatitude(); // Exposed to Python
	int getBottomLatitude(); // Exposed to Python

	int	 getNextRiverID(); // Exposed to Python
	void incrementNextRiverID(); // Exposed to Python

	bool isWrapX() const; // Exposed to Python
#ifdef _USRDLL
	inline bool isWrapXINLINE() const
	{
		return m_bWrapX;
	}
#endif
	bool isWrapY() const; // Exposed to Python
#ifdef _USRDLL
	inline bool isWrapYINLINE() const
	{
		return m_bWrapY;
	}
#endif
	bool isWrap() const;
#ifdef _USRDLL
	inline bool isWrapINLINE() const
	{
		return m_bWrapX || m_bWrapY;
	}
#endif
	WorldSizeTypes getWorldSize(); // Exposed to Python
	ClimateTypes   getClimate(); // Exposed to Python
	SeaLevelTypes  getSeaLevel(); // Exposed to Python

	int					 getNumCustomMapOptions();
	CustomMapOptionTypes getCustomMapOption(int iOption); // Exposed to Python

	int	 getNumBonuses(BonusTypes eIndex); // Exposed to Python
	void changeNumBonuses(BonusTypes eIndex, int iChange);

	int	 getNumBonusesOnLand(BonusTypes eIndex); // Exposed to Python
	void changeNumBonusesOnLand(BonusTypes eIndex, int iChange);

	CvPlot* plotByIndex(int iIndex) const; // Exposed to Python
#ifdef _USRDLL
	inline CvPlot* plotByIndexINLINE(int iIndex) const
	{
		return (((iIndex >= 0) && (iIndex < (getGridWidthINLINE() * getGridHeightINLINE()))) ? &(m_pMapPlots[iIndex]) : NULL);
	}
#endif
	CvPlot* plot(int iX, int iY) const; // Exposed to Python
#ifdef _USRDLL
	__forceinline CvPlot* plotINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		int iMapX = coordRange(iX, getGridWidthINLINE(), isWrapXINLINE());
		int iMapY = coordRange(iY, getGridHeightINLINE(), isWrapYINLINE());
		return ((isPlotINLINE(iMapX, iMapY)) ? &(m_pMapPlots[plotNumINLINE(iMapX, iMapY)]) : NULL);
	}
	__forceinline CvPlot* plotSorenINLINE(int iX, int iY) const
	{
		if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
		{
			return NULL;
		}
		return &(m_pMapPlots[plotNumINLINE(iX, iY)]);
	}
#endif
	CvPlot* pointToPlot(float fX, float fY);

	int		getIndexAfterLastArea(); // Exposed to Python
	int		getNumAreas(); // Exposed to Python
	int		getNumLandAreas();
	CvArea* getArea(int iID); // Exposed to Python
	CvArea* addArea();
	void	deleteArea(int iID);
	// iteration
	CvArea* firstArea(int* pIterIdx, bool bRev = false); // Exposed to Python
	CvArea* nextArea(int* pIterIdx, bool bRev = false); // Exposed to Python

	void recalculateAreas(); // Exposed to Python

	void resetPathDistance(); // Exposed to Python
	// Super Forts begin *canal* *choke*
	int	 calculatePathDistance(CvPlot* pSource, CvPlot* pDest, CvPlot* pInvalidPlot = NULL); // Exposed to Python
	void calculateCanalAndChokePoints(); // Exposed to Python
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

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes* eCustomMapOptions); // Exposed to Python

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
	MapTypes				 m_eType;
	std::vector<CvViewport*> m_viewports;
	int						 m_iCurrentViewportIndex;
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

  public:
	// AIAndy: Expose path generation functionality here to expose it to Python via CyMap
	bool	generatePathForHypotheticalUnit(const CvPlot* pFrom, const CvPlot* pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags, int iMaxTurns);
	CvPath& getLastPath();
	int		getLastPathStepNum();
	CvPlot* getLastPathPlotByIndex(int index);
};

#endif
