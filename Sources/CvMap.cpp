//
//	FILE:	 CvMap.cpp
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvFractal.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvMapGenerator.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPlotGroup.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvUnit.h"
#include "CvViewport.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "FAStarNode.h"
#include <direct.h> // for getcwd()
#include <stdlib.h> // for MAX_PATH

bool CvMap::m_bSwitchInProgress = false;


CvMap::CvMap(MapTypes eType)
	: m_iGridWidth(0)
	, m_iGridHeight(0)
	, m_iLandPlots(0)
	, m_iOwnedPlots(0)
	, m_iTopLatitude(0)
	, m_iBottomLatitude(0)
	, m_iNextRiverID(0)
	, m_eType(eType)
	, m_iCurrentViewportIndex(0)
	, m_bWrapX(false)
	, m_bWrapY(false)
	, m_paiNumBonus(NULL)
	, m_paiNumBonusOnLand(NULL)
	, m_bCitiesDisplayed(true)
	, m_bUnitsDisplayed(true)
	, m_pMapPlots(NULL)
{
	OutputDebugString("Calling constructor for Map: Start\n");

	CvMapInitData defaultMapData;
	reset(&defaultMapData);

	OutputDebugString("Calling constructor for Map: End\n");
}


CvMap::~CvMap()
{
	uninit();
}

// FUNCTION: init()
// Initializes the map.
// Parameters:
//	pInitInfo					- Optional init structure (used for WB load)
// Returns:
//	nothing.
void CvMap::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	OutputDebugString("Initializing Map: Start\n");
	PROFILE("CvMap::init");
	gDLL->logMemState( CvString::format("CvMap::init begin - world size=%s, climate=%s, sealevel=%s, num custom options=%6",
		GC.getWorldInfo(GC.getInitCore().getWorldSize()).getDescription(),
		GC.getClimateInfo(GC.getInitCore().getClimate()).getDescription(),
		GC.getSeaLevelInfo(GC.getInitCore().getSeaLevel()).getDescription(),
		GC.getInitCore().getNumCustomMapOptions()).c_str() );

	Cy::call_optional(getMapScript(), "beforeInit");

	//--------------------------------
	// Init saved data
	reset(pInitInfo);

	//--------------------------------
	// Init containers
	m_areas.init();

	//--------------------------------
	// Init non-saved data
	setup();

	//--------------------------------
	// Init other game data
	gDLL->logMemState("CvMap before init plots");
	m_pMapPlots = new CvPlot[numPlots()];
	for (int iX = 0; iX < getGridWidth(); iX++)
	{
		gDLL->callUpdater();
		for (int iY = 0; iY < getGridHeight(); iY++)
		{
			plotSorenINLINE(iX, iY)->init(iX, iY);
		}
	}
	calculateAreas();
	gDLL->logMemState("CvMap after init plots");

	OutputDebugString("Initializing Map: End\n");
}


void CvMap::uninit()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_paiNumBonus);
	SAFE_DELETE_ARRAY(m_paiNumBonusOnLand);

	SAFE_DELETE_ARRAY(m_pMapPlots);

	m_areas.uninit();

	foreach_(const CvViewport* viewport, m_viewports)
	{
		delete viewport;
	}
	m_viewports.clear();

	foreach_(const TravelingUnit* unit, m_IncomingUnits)
	{
		delete unit;
	}
	m_IncomingUnits.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
	PROFILE_EXTRA_FUNC();
	//--------------------------------
	// Uninit class
	uninit();

	//
	// set grid size
	// initially set in terrain cell units
	//
	m_iGridWidth = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridWidth (): 0;	//todotw:tcells wide
	m_iGridHeight = (GC.getInitCore().getWorldSize() != NO_WORLDSIZE) ? GC.getWorldInfo(GC.getInitCore().getWorldSize()).getGridHeight (): 0;

	// allow grid size override
	if (pInitInfo)
	{
		m_iGridWidth	= pInitInfo->m_iGridW;
		m_iGridHeight	= pInitInfo->m_iGridH;
	}
	else
	{
		// check map script for grid size override
		if (GC.getInitCore().getWorldSize() != NO_WORLDSIZE)
		{
			std::vector<int> out;
			if (Cy::call_override(getMapScript(), "getGridSize", Cy::Args() << GC.getInitCore().getWorldSize(), out)
				&& out.size() == 2)
			{
				m_iGridWidth = out[0];
				m_iGridHeight = out[1];
				FAssertMsg(m_iGridWidth > 0 && m_iGridHeight > 0, "The width and height returned by python getGridSize() must be positive");
			}
		}

		// convert to plot dimensions
		if (GC.getNumLandscapeInfos() > 0)
		{
			m_iGridWidth *= GC.getLandscapeInfo(GC.getActiveLandscapeID()).getPlotsPerCellX();
			m_iGridHeight *= GC.getLandscapeInfo(GC.getActiveLandscapeID()).getPlotsPerCellY();
		}
	}

	m_iLandPlots = 0;
	m_iOwnedPlots = 0;

	if (pInitInfo)
	{
		m_iTopLatitude = pInitInfo->m_iTopLatitude;
		m_iBottomLatitude = pInitInfo->m_iBottomLatitude;
	}
	else
	{
		// Check map script for latitude override (map script beats ini file)
		long resultTop = 0, resultBottom = 0;
		if(Cy::call_override(getMapScript(), "getTopLatitude", resultTop)
			&& Cy::call_override(getMapScript(), "getBottomLatitude", resultBottom))
		{
			m_iTopLatitude = resultTop;
			m_iBottomLatitude = resultBottom;
		}
	}

	m_iTopLatitude = std::min(m_iTopLatitude, 90);
	m_iTopLatitude = std::max(m_iTopLatitude, -90);
	m_iBottomLatitude = std::min(m_iBottomLatitude, 90);
	m_iBottomLatitude = std::max(m_iBottomLatitude, -90);

	m_iNextRiverID = 0;

	//
	// set wrapping
	//
	m_bWrapX = true;
	m_bWrapY = false;
	if (pInitInfo)
	{
		m_bWrapX = pInitInfo->m_bWrapX;
		m_bWrapY = pInitInfo->m_bWrapY;
	}
	else
	{
		// Check map script for wrap override (map script beats ini file)
		long resultX = 0, resultY = 0;
		if (Cy::call_override(getMapScript(), "getWrapX", resultX)
			&& Cy::call_override(getMapScript(), "getWrapY", resultY))
		{
			m_bWrapX = (resultX != 0);
			m_bWrapY = (resultY != 0);
		}
	}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	if (m_eType > MAP_EARTH && GC.getNumMapInfos() > 0)
	{
		if (GC.getMapInfo(getType()).getGridWidth() > 0 && GC.getMapInfo(getType()).getGridHeight() > 0)
		{
			m_iGridWidth = GC.getMapInfo(getType()).getGridWidth();
			m_iGridHeight = GC.getMapInfo(getType()).getGridHeight();
		}
		if (GC.getMapInfo(getType()).getWrapX() > 0 && GC.getMapInfo(getType()).getWrapY() > 0)
		{
			m_bWrapX = GC.getMapInfo(getType()).getWrapX();
			m_bWrapY = GC.getMapInfo(getType()).getWrapY();
		}
	}
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	if (GC.getNumBonusInfos())
	{
		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvMap::reset");
		FAssertMsg(m_paiNumBonus==NULL, "mem leak m_paiNumBonus");
		m_paiNumBonus = new int[GC.getNumBonusInfos()];
		FAssertMsg(m_paiNumBonusOnLand==NULL, "mem leak m_paiNumBonusOnLand");
		m_paiNumBonusOnLand = new int[GC.getNumBonusInfos()];
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiNumBonus[iI] = 0;
			m_paiNumBonusOnLand[iI] = 0;
		}
	}

	m_areas.removeAll();

	//	Create a viewport of the requisite external size without initial positioning (this can be repositioned
	//	at any time before it is graphically initialised, of after with a map switch)
	addViewport(-1, -1);

	if ( !GC.viewportsEnabled() )
	{
		//	If no viewports are exposed there will only ever be one, and it's exactly aligned to the map
		getCurrentViewport()->setMapOffset(0, 0);
	}
}


// FUNCTION: setup()
// Initializes all data that is not serialized but needs to be initialized after loading.
void CvMap::setup()
{
	PROFILE("CvMap::setup");

	gDLL->getFAStarIFace()->Initialize(&GC.getPathFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getInterfacePathFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), pathDestValid, pathHeuristic, pathCost, pathValid, pathAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getStepFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), stepDestValid, stepHeuristic, stepCost, stepValid, stepAdd, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getRouteFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), NULL, NULL, NULL, routeValid, NULL, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getBorderFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), NULL, NULL, NULL, borderValid, NULL, NULL, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getAreaFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), NULL, NULL, NULL, areaValid, NULL, joinArea, NULL);
	gDLL->getFAStarIFace()->Initialize(&GC.getPlotGroupFinder(), getGridWidth(), getGridHeight(), isWrapX(), isWrapY(), NULL, NULL, NULL, plotGroupValid, NULL, countPlotGroup, NULL);
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvMap::setupGraphical()
{
	PROFILE_FUNC();

	if (GC.IsGraphicsInitialized() && m_pMapPlots != NULL)
	{
		for (int iI = 0; iI < numPlots(); iI++)
		{
			if ( (iI % 10) == 0 )
			{
				gDLL->callUpdater();	// allow windows msgs to update
			}
			plotByIndex(iI)->setupGraphical();
		}
	}
}


void CvMap::erasePlots()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->erase();
	}
}


void CvMap::setRevealedPlots(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->setRevealed(eTeam, bNewValue, bTerrainOnly, NO_TEAM, false);
	}

	GC.getGame().updatePlotGroups();
}


void CvMap::resetRevealedPlots(TeamTypes eTeam)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		if (!plotByIndex(iI)->isVisible(eTeam, false))
		{
			plotByIndex(iI)->setRevealed(eTeam, false, false, NO_TEAM, false);
		}
	}

	GC.getGame().updatePlotGroups();
}


void CvMap::setAllPlotTypes(PlotTypes ePlotType)
{
	PROFILE_EXTRA_FUNC();
	//float startTime = (float) timeGetTime();

	for(int i = 0; i < numPlots(); i++)
	{
		plotByIndex(i)->setPlotType(ePlotType, false, false);
	}

	recalculateAreas();

	//rebuild landscape
	gDLL->getEngineIFace()->RebuildAllPlots();

	//mark minimap as dirty
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);

	//float endTime = (float) timeGetTime();
	//OutputDebugString(CvString::format("[Jason] setAllPlotTypes: %f\n", endTime - startTime).c_str());
}


void CvMap::moveUnitToMap(CvUnit& unit, int numTravelTurns)
{
	m_IncomingUnits.push_back(new TravelingUnit(unit, numTravelTurns));
	unit.kill(true, NO_PLAYER);
}

void CvMap::updateIncomingUnits()
{
	PROFILE_EXTRA_FUNC();
	foreach_(TravelingUnit* travelingUnit, m_IncomingUnits)
	{
		if (travelingUnit->numTurnsUntilArrival-- <= 0)
		{
			GC.switchMap(m_eType);

			const CvUnitAI& unit = travelingUnit->unit;
			CvPlayer& owner = GET_PLAYER(unit.getOwner());
			const CvPlot* plot = owner.findStartingPlot();
			CvUnit* newUnit = owner.initUnit(unit.getUnitType(), plot->getX(), plot->getY(), unit.AI_getUnitAIType(), NO_DIRECTION, 0);
			if (newUnit != NULL)
			{
				static_cast<CvUnitAI&>(*newUnit) = unit;
				m_IncomingUnits.erase(&travelingUnit);
				delete travelingUnit;
			}
		}
	}
}


void CvMap::doTurn()
{
	PROFILE("CvMap::doTurn()");

	if (plotsInitialized())
	{
		GC.switchMap(m_eType);

		updateIncomingUnits();

		for (int iI = 0; iI < numPlots(); iI++)
		{
			plotByIndex(iI)->doTurn();
		}
	}
}


void CvMap::updateFlagSymbols()
{
	updateFlagSymbolsInternal(false);
}

void CvMap::updateFlagSymbolsInternal(bool bForce)
{
	PROFILE_FUNC();

	foreach_(CvPlot& plotX, plots())
	{
		if (bForce || plotX.isFlagDirty())
		{
			plotX.updateFlagSymbol();
			plotX.setFlagDirty(false);
		}
	}
}


void CvMap::updateFog()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateFog();
	}
}


void CvMap::updateVisibility()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateVisibility();
	}
}


void CvMap::updateSymbolVisibility()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateSymbolVisibility();
	}
}


void CvMap::updateSymbols()
{
	PROFILE_FUNC();

	//	Ignore this while we are demand-creating symbols to minimize memory usage - REMOVED FOR NOW FOR VIEWPORTS

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateSymbols();
	}
}


void CvMap::updateMinimapColor()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateMinimapColor();
	}
}


void CvMap::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateSight(bIncrement, false);
	}

	if ( bUpdatePlotGroups )
	{
		GC.getGame().updatePlotGroups();
	}
}


void CvMap::updateIrrigated()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateIrrigated();
	}
}


void CvMap::updateCenterUnit()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateCenterUnit();
	}
}


void CvMap::updateWorkingCity()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateWorkingCity();
	}
}


void CvMap::updateMinOriginalStartDist(const CvArea* pArea)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndex(iI);

		if (pLoopPlot->area() == pArea)
		{
			pLoopPlot->setMinOriginalStartDist(-1);
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

		if (pStartingPlot != NULL && pStartingPlot->area() == pArea)
		{
			for (int iJ = 0; iJ < numPlots(); iJ++)
			{
				CvPlot* pLoopPlot = plotByIndex(iJ);

				if (pLoopPlot->area() == pArea)
				{
					const int iDist = stepDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

					if (iDist != -1)
					{
						if (pLoopPlot->getMinOriginalStartDist() == -1 || iDist < pLoopPlot->getMinOriginalStartDist())
						{
							pLoopPlot->setMinOriginalStartDist(iDist);
						}
					}
				}
			}
		}
	}
}


void CvMap::updateYield()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateYield();
	}
}


void CvMap::verifyUnitValidPlot()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->verifyUnitValidPlot();
	}
}


void CvMap::combinePlotGroups(CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses)
{
	FAssertMsg(pPlotGroup1 != NULL, "pPlotGroup is not assigned to a valid value");
	FAssertMsg(pPlotGroup2 != NULL, "pPlotGroup is not assigned to a valid value");

	if (pPlotGroup1 == pPlotGroup2)
	{
		return;
	}

	if (pPlotGroup1->getLengthPlots() > pPlotGroup2->getLengthPlots())
	{
		pPlotGroup1->mergeIn(pPlotGroup2, bRecalculateBonuses);
	}
	else
	{
		pPlotGroup2->mergeIn(pPlotGroup1, bRecalculateBonuses);
	}
}

CvPlot* CvMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	PROFILE_EXTRA_FUNC();
	CvPlot* pPlot = NULL;
	int iCount = 0;

	while (iCount < iTimeout)
	{
		CvPlot* pTestPlot = plotSorenINLINE(GC.getGame().getMapRandNum(getGridWidth(), "Rand Plot Width"), GC.getGame().getMapRandNum(getGridHeight(), "Rand Plot Height"));

		FAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if ((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bool bValid = true;

			if (iMinUnitDistance != -1
			&& algo::any_of(pTestPlot->rect(iMinUnitDistance, iMinUnitDistance), bind(&CvPlot::isUnit, _1)))
			{
				bValid = false;
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_LAND)
				{
					if (pTestPlot->isWater())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_UNOWNED)
				{
					if (pTestPlot->isOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_UNOWNED)
				{
					if (pTestPlot->isAdjacentOwned())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_ADJACENT_LAND)
				{
					if (!(pTestPlot->isAdjacentToLand()))
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_PASSIBLE)
				{
					if (pTestPlot->isImpassable())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_VISIBLE_TO_CIV)
				{
					if (pTestPlot->isVisibleToCivTeam())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				if (iFlags & RANDPLOT_NOT_CITY)
				{
					if (pTestPlot->isCity())
					{
						bValid = false;
					}
				}
			}

			if (bValid)
			{
				pPlot = pTestPlot;
				break;
			}
		}

		iCount++;
	}

	return pPlot;
}


CvCity* CvMap::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, const CvCity* pSkipCity) const
{
	PROFILE_FUNC();

	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (eOwner == NO_PLAYER || iI == eOwner)
		{
			const CvPlayer& player = GET_PLAYER((PlayerTypes)iI);

			if (!player.isAliveAndTeam(eTeam))
			{
				continue;
			}
			// eTeam may be NO_TEAM, this is ok.

			foreach_(CvCity* pLoopCity, player.cities())
			{
				if (
					(!bSameArea || pLoopCity->area() == plot(iX, iY)->area() || bCoastalOnly && pLoopCity->waterArea() == plot(iX, iY)->area())
				&&
					(!bCoastalOnly || pLoopCity->isCoastal(GC.getWorldInfo(getWorldSize()).getOceanMinAreaSize()))
				&&
					(eTeamAtWarWith == NO_TEAM || atWar(player.getTeam(), eTeamAtWarWith))
				&&
					(eDirection == NO_DIRECTION || estimateDirection(dxWrap(pLoopCity->getX() - iX), dyWrap(pLoopCity->getY() - iY)) == eDirection)
				&&
					(pSkipCity == NULL || pLoopCity != pSkipCity))
				{
					const int iValue = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());
					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}
	return pBestCity;
}


CvSelectionGroup* CvMap::findSelectionGroup(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers) const
{
	return findSelectionGroupInternal(iX, iY, eOwner, bReadyToSelect, bWorkers, false);
}

CvSelectionGroup* CvMap::findSelectionGroupInternal(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers, bool bAllowViewportSwitch) const
{
	PROFILE_EXTRA_FUNC();
	// XXX look for barbarian cities???

	int iBestValue = MAX_INT;
	int iBestValueInViewport = MAX_INT;
	CvSelectionGroup* pBestSelectionGroup = NULL;
	CvSelectionGroup* pBestSelectionGroupInViewport = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && (eOwner == NO_PLAYER || iI == eOwner))
		{
			foreach_(CvSelectionGroup* pLoopGroup, GET_PLAYER((PlayerTypes)iI).groups())
			{
				if ((!bReadyToSelect || pLoopGroup->readyToSelect()) && (!bWorkers || pLoopGroup->hasWorker()))
				{
					const int iValue = plotDistance(iX, iY, pLoopGroup->getX(), pLoopGroup->getY());

					if (getCurrentViewport()->isInViewport(pLoopGroup->getX(), pLoopGroup->getY()))
					{
						if (iValue < iBestValueInViewport)
						{
							iBestValueInViewport = iValue;
							pBestSelectionGroupInViewport = pLoopGroup;
						}
					}

					if (iValue < iBestValue)
					{
						iBestValue = iValue;
						pBestSelectionGroup = pLoopGroup;
					}
				}
			}
		}
	}

	return pBestSelectionGroupInViewport != NULL ? pBestSelectionGroupInViewport : (bAllowViewportSwitch ? pBestSelectionGroup : NULL);
}

CvArea* CvMap::findBiggestArea(bool bWater) const
{
	return scoring::max_score(
		areas() | filtered(CvArea::fn::isWater() == bWater),
		CvArea::fn::getNumTiles()
	).get_value_or(nullptr);
}


int CvMap::getMapFractalFlags() const
{
	const int wrapX = isWrapX() ? (int)CvFractal::FRAC_WRAP_X : 0;
	const int wrapY = isWrapY() ? (int)CvFractal::FRAC_WRAP_Y : 0;
	return (wrapX | wrapY);
}


//"Check plots for wetlands or seaWater.  Returns true if found"
bool CvMap::findWater(const CvPlot* pPlot, int iRange, bool bFreshWater) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pLoopPlot, pPlot->rect(iRange, iRange))
	{
		if (bFreshWater ? pLoopPlot->isFreshWater() : pLoopPlot->isWater())
		{
			return true;
		}
	}

	return false;
}


int CvMap::plotX(int iIndex) const
{
	return (iIndex % getGridWidth());
}


int CvMap::plotY(int iIndex) const
{
	return (iIndex / getGridWidth());
}


int CvMap::pointXToPlotX(float fX) const
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fX + (fWidth/2.0f)) / fWidth) * getGridWidth());
}


float CvMap::plotXToPointX(int iX) const
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iX * fWidth) / ((float)getGridWidth())) - (fWidth / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


int CvMap::pointYToPlotY(float fY) const
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return (int)(((fY + (fHeight/2.0f)) / fHeight) * getGridHeight());
}


float CvMap::plotYToPointY(int iY) const
{
	float fWidth, fHeight;
	gDLL->getEngineIFace()->GetLandscapeGameDimensions(fWidth, fHeight);
	return ((iY * fHeight) / ((float)getGridHeight())) - (fHeight / 2.0f) + (GC.getPLOT_SIZE() / 2.0f);
}


float CvMap::getWidthCoords() const
{
	return (GC.getPLOT_SIZE() * (float)getGridWidth());
}


float CvMap::getHeightCoords() const
{
	return (GC.getPLOT_SIZE() * (float)getGridHeight());
}


int CvMap::maxPlotDistance() const
{
	return std::max(1, plotDistance(0, 0, (isWrapX() ? (getGridWidth() / 2) : (getGridWidth() - 1)), (isWrapY() ? (getGridHeight() / 2) : (getGridHeight() - 1))));
}


int CvMap::maxStepDistance() const
{
	return std::max(1, stepDistance(0, 0, (isWrapX() ? (getGridWidth() / 2) : (getGridWidth() - 1)), (isWrapY() ? (getGridHeight() / 2) : (getGridHeight() - 1))));
}


int CvMap::getLandPlots() const
{
	return m_iLandPlots;
}


void CvMap::changeLandPlots(int iChange)
{
	m_iLandPlots += iChange;
	FASSERT_NOT_NEGATIVE(getLandPlots());
}


int CvMap::getOwnedPlots() const
{
	return m_iOwnedPlots;
}


void CvMap::changeOwnedPlots(int iChange)
{
	m_iOwnedPlots += iChange;
	FASSERT_NOT_NEGATIVE(getOwnedPlots());
}


int CvMap::getTopLatitude() const
{
	return m_iTopLatitude;
}


int CvMap::getBottomLatitude() const
{
	return m_iBottomLatitude;
}


int CvMap::getNextRiverID() const
{
	return m_iNextRiverID;
}


void CvMap::incrementNextRiverID()
{
	m_iNextRiverID++;
}


WorldSizeTypes CvMap::getWorldSize() const
{
	return GC.getInitCore().getWorldSize();
}


ClimateTypes CvMap::getClimate() const
{
	return GC.getInitCore().getClimate();
}


SeaLevelTypes CvMap::getSeaLevel() const
{
	return GC.getInitCore().getSeaLevel();
}


int CvMap::getNumCustomMapOptions() const
{
	return GC.getInitCore().getNumCustomMapOptions();
}


CustomMapOptionTypes CvMap::getCustomMapOption(int iOption) const
{
	return GC.getInitCore().getCustomMapOption(iOption);
}


int CvMap::getNumBonuses(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	return m_paiNumBonus[eIndex];
}


void CvMap::changeNumBonuses(BonusTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	m_paiNumBonus[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(m_paiNumBonus[eIndex]);
}


int CvMap::getNumBonusesOnLand(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	return m_paiNumBonusOnLand[eIndex];
}


void CvMap::changeNumBonusesOnLand(BonusTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	m_paiNumBonusOnLand[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getNumBonusesOnLand(eIndex));
}


CvPlot* CvMap::pointToPlot(float fX, float fY) const
{
	return plot(pointXToPlotX(fX), pointYToPlotY(fY));
}


int CvMap::getIndexAfterLastArea() const
{
	return m_areas.getIndexAfterLast();
}


int CvMap::getNumAreas() const
{
	return m_areas.getCount();
}


int CvMap::getNumLandAreas() const
{
	return algo::count_if(areas(), !bind(CvArea::isWater, _1));
}


CvArea* CvMap::getArea(int iID) const
{
	return m_areas.getAt(iID);
}


CvArea* CvMap::addArea()
{
	return m_areas.add();
}


void CvMap::deleteArea(int iID)
{
	m_areas.removeAt(iID);
}


CvArea* CvMap::firstArea(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_areas.beginIter(pIterIdx) : m_areas.endIter(pIterIdx);
}


CvArea* CvMap::nextArea(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_areas.nextIter(pIterIdx) : m_areas.prevIter(pIterIdx);
}


void CvMap::recalculateAreas()
{
	PROFILE("CvMap::recalculateAreas");

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->setArea(FFreeList::INVALID_INDEX);
	}

	m_areas.removeAll();

	calculateAreas();
}


void CvMap::resetPathDistance()
{
	gDLL->getFAStarIFace()->ForceReset(&GC.getStepFinder());
}


// Super Forts begin *canal* *choke*
int CvMap::calculatePathDistance(const CvPlot* pSource, const CvPlot* pDest, const CvPlot* pInvalidPlot) const
// Super Forts end
{
	PROFILE_FUNC();

	if (pSource == NULL || pDest == NULL)
	{
		return -1;
	}

	// Super Forts begin *canal* *choke*
	// 1 must be added because 0 is already being used as the default value for iInfo in GeneratePath()
	const int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : plotNum(pInvalidPlot->getX(), pInvalidPlot->getY()) + 1;

	if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSource->getX(), pSource->getY(), pDest->getX(), pDest->getY(), false, iInvalidPlot, true))
	// Super Forts end
	{
		const FAStarNode* pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

		if (pNode != NULL)
		{
			return pNode->m_iData1;
		}
	}

	return -1;
}


// Super Forts begin *canal* *choke*
void CvMap::calculateCanalAndChokePoints()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->calculateCanalValue();
		plotByIndex(iI)->calculateChokeValue();
		// TEMPORARY HARD CODE for testing purposes
		/*if((plotByIndex(iI)->getChokeValue() > 0) || (plotByIndex(iI)->getCanalValue() > 0))
		{
			ImprovementTypes eImprovement = (ImprovementTypes) (plotByIndex(iI)->isWater() ? GC.getInfoTypeForString("IMPROVEMENT_OFFSHORE_PLATFORM") : GC.getInfoTypeForString("IMPROVEMENT_FORT"));
			plotByIndex(iI)->setImprovementType(eImprovement);
		}
		else
		{
			plotByIndex(iI)->setImprovementType(NO_IMPROVEMENT);
		}*/
	}
}
// Super Forts end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
// Plot danger cache
void CvMap::invalidateIsActivePlayerNoDangerCache()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndex(iI);

		pLoopPlot->setIsActivePlayerNoDangerCache(false);
		pLoopPlot->setIsActivePlayerHasDangerCache(false);
		pLoopPlot->CachePathValidityResult(NULL,false,false);
		pLoopPlot->CachePathValidityResult(NULL,true,false);
	}
}


void CvMap::invalidateIsTeamBorderCache(TeamTypes eTeam)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->setIsTeamBorderCache(eTeam, false);
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


const std::pair<CvPlot*, CvPlot*> CvMap::plots() const
{
	return std::make_pair(&m_pMapPlots[0], &m_pMapPlots[numPlots()]);
}


//
// read object from a stream
// used during load
//
void CvMap::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString("Reading Map: Start\n");
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvMapInitData defaultMapData;

	// Init data before load
	reset(&defaultMapData);

	WRAPPER_READ(wrapper, "CvMap", &m_iGridWidth);
	WRAPPER_READ(wrapper, "CvMap", &m_iGridHeight);

	getCurrentViewport()->resizeForMap();

	WRAPPER_READ(wrapper, "CvMap", &m_iLandPlots);
	WRAPPER_READ(wrapper, "CvMap", &m_iOwnedPlots);
	WRAPPER_READ(wrapper, "CvMap", &m_iTopLatitude);
	WRAPPER_READ(wrapper, "CvMap", &m_iBottomLatitude);
	WRAPPER_READ(wrapper, "CvMap", &m_iNextRiverID);

	WRAPPER_READ(wrapper, "CvMap", &m_bWrapX);
	WRAPPER_READ(wrapper, "CvMap", &m_bWrapY);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonus);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonusOnLand);

	if (numPlots() > 0)
	{
		m_pMapPlots = new CvPlot[numPlots()];

		for (int iI = 0; iI < numPlots(); iI++)
		{
			m_pMapPlots[iI].read(pStream);
		}
	}

	// call the read of the free list CvArea class allocations
	ReadStreamableFFreeListTrashArray(m_areas, pStream);

	setup();

	WRAPPER_READ_OBJECT_END(wrapper);

	OutputDebugString("Reading Map: End\n");
}

// save object to a stream
// used during save
//
void CvMap::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvMap", m_iGridWidth);
	WRAPPER_WRITE(wrapper, "CvMap", m_iGridHeight);
	WRAPPER_WRITE(wrapper, "CvMap", m_iLandPlots);
	WRAPPER_WRITE(wrapper, "CvMap", m_iOwnedPlots);
	WRAPPER_WRITE(wrapper, "CvMap", m_iTopLatitude);
	WRAPPER_WRITE(wrapper, "CvMap", m_iBottomLatitude);
	WRAPPER_WRITE(wrapper, "CvMap", m_iNextRiverID);

	WRAPPER_WRITE(wrapper, "CvMap", m_bWrapX);
	WRAPPER_WRITE(wrapper, "CvMap", m_bWrapY);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonusOnLand);

	for (int iI = 0; iI < numPlots(); iI++)
	{
		m_pMapPlots[iI].write(pStream);
	}

	// call the read of the free list CvArea class allocations
	WriteStreamableFFreeListTrashArray(m_areas, pStream);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvMap::beforeSwitch()
{
	PROFILE_FUNC();

	m_bSwitchInProgress = true;

#ifdef THE_GREAT_WALL
	if (getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW)
	{
		GC.getGame().processGreatWall(false);
	}
#endif // THE_GREAT_WALL

	gDLL->getEngineIFace()->setResourceLayer(false);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)i).units()
			| filtered(!bind(&CvUnit::isUsingDummyEntities, _1)))
			{
				gDLL->getEntityIFace()->RemoveUnitFromBattle(pLoopUnit);

				pLoopUnit->removeEntity();
				pLoopUnit->destroyEntity();
			}
			foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)i).cities()
			| filtered(bind(CvCity::getEntity, _1) != nullptr))
			{
				pLoopCity->removeEntity();
				pLoopCity->destroyEntity();
			}
		}
	}

	gDLL->getInterfaceIFace()->clearSelectionList();
	gDLL->getInterfaceIFace()->clearSelectedCities();

	GC.clearSigns();

	for (int i = 0; i < numPlots(); i++)
	{
		plotByIndex(i)->destroyGraphics();
	}
}

void CvMap::afterSwitch()
{
	PROFILE_FUNC();

	if (!plotsInitialized())
	{
		if (!GC.getMapInfo(getType()).getInitialWBMap().empty())
		{
			char mapPath[1024];
			getcwd(mapPath, 1024);
			strcat(mapPath, static_cast<const char*>(GC.getMapInfo(getType()).getInitialWBMap()));
			Cy::call("CvWBInterface", "readAndApplyDesc", Cy::Args() << mapPath);
		}
		else
		{
			init();
			CvMapGenerator& kGenerator = CvMapGenerator::GetInstance();
			kGenerator.generateRandomMap();
			kGenerator.addGameElements();
		}
	}

	gDLL->getEngineIFace()->InitGraphics(); // Matt: Need this to resize map, but it creates a bug with the fog.
	gDLL->getEngineIFace()->updateFoundingBorder(); // Matt: Maybe need this.
	gDLL->getEngineIFace()->MarkBridgesDirty(); // Matt: Maybe need this.
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->makeSelectionListDirty();
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(BlockadedPlots_DIRTY_BIT, true); // Matt: Maybe need this.
	gDLL->getInterfaceIFace()->setDirty(Fog_DIRTY_BIT, true); // Matt: Maybe need this.

	int iWidth = GC.getMapInfo(getType()).getGridWidth();
	if (iWidth == 0)
	{
		iWidth = MAX_INT;
	}

	int iHeight = GC.getMapInfo(getType()).getGridHeight();
	if (iHeight == 0)
	{
		iHeight = MAX_INT;
	}

	for (int i = 0; i < numPlots(); i++)
	{
		//	Koshlimg - this is no longer necesary (or correct) with viewports enabled
		if (!GC.viewportsEnabled() && (plotByIndex(i)->getX() > iWidth || plotByIndex(i)->getY() > iHeight))
		{
			plotByIndex(i)->setNull(true);
		}
		else
		{
			plotByIndex(i)->setLayoutDirty(true);
			plotByIndex(i)->setFlagDirty(true);
		}
	}

	gDLL->getEngineIFace()->RebuildAllPlots();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)i).cities())
			{
				//gDLL->getEntityIFace()->createCityEntity(pLoopCity);
				pLoopCity->setupGraphical();
			}
			foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)i).units()
			| filtered(!bind(CvUnit::isUsingDummyEntities, _1)))
			{
				gDLL->getEntityIFace()->createUnitEntity(pLoopUnit);
				pLoopUnit->setupGraphical();
			}
		}
	}

	setupGraphical();
	updateFog();
	updateSymbols();
	updateFlagSymbols();
	updateMinimapColor();

	GC.reprocessSigns();

#ifdef THE_GREAT_WALL
	if (getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW)
	{
		GC.getGame().processGreatWall(true);
	}
#endif // THE_GREAT_WALL

	gDLL->getEngineIFace()->setResourceLayer(GC.getResourceLayer());

	m_bSwitchInProgress = false;
}

const std::vector<CvViewport*>& CvMap::getViewports() const
{
	return m_viewports;
}

void CvMap::addViewport(int iXOffset, int iYOffset)
{
	CvViewport*	viewport = new CvViewport(this);

	if (iXOffset >= 0 && iYOffset >= 0)
	{
		viewport->setMapOffset(iXOffset, iYOffset);
	}
	m_viewports.push_back(viewport);
}

/*
void CvMap::deleteViewport(int iIndex)
{
	FASSERT_BOUNDS(0, m_viewports.size(), iIndex);

	if (m_iCurrentViewportIndex == iIndex)
	{
		m_iCurrentViewportIndex = -1;
	}

	delete m_viewports[iIndex];

	for (uint32_t iI = iIndex; iI < m_viewports.size()-1; iI++)
	{
		m_viewports[iI] = m_viewports[iI+1];
	}

	m_viewports.pop_back();
}
*/

CvViewport* CvMap::getCurrentViewport() const
{
	FASSERT_BOUNDS(0, (int)m_viewports.size(), m_iCurrentViewportIndex);
	return m_viewports[m_iCurrentViewportIndex];
}

MapTypes CvMap::getType() const
{
	return m_eType;
}

const char* CvMap::getMapScript() const
{
	if (m_eType > MAP_EARTH)
	{
		const CvString& module = GC.getMapInfo(m_eType).getMapScript();
		if (module.GetLength() > 0)
			return module.c_str();
	}
	return gDLL->getPythonIFace()->getMapScriptModule();
}

bool CvMap::plotsInitialized() const
{
	return m_pMapPlots != NULL;
}

//
// used for loading WB maps
//
void CvMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	CvMapInitData initData(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY);

	// Set init core data
	GC.getInitCore().setWorldSize(eWorldSize);
	GC.getInitCore().setClimate(eClimate);
	GC.getInitCore().setSeaLevel(eSeaLevel);
	GC.getInitCore().setCustomMapOptions(iNumCustomMapOptions, aeCustomMapOptions);

	// Init map
	init(&initData);
}


//////////////////////////////////////////////////////////////////////////
// Protected Functions...
//////////////////////////////////////////////////////////////////////////

void CvMap::calculateAreas()
{
	PROFILE("CvMap::calculateAreas");

	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndex(iI);
		gDLL->callUpdater();
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (pLoopPlot->getArea() == FFreeList::INVALID_INDEX)
		{
			CvArea* pArea = addArea();
			pArea->init(pArea->getID(), pLoopPlot->isWater());

			const int iArea = pArea->getID();

			pLoopPlot->setArea(iArea);

			gDLL->getFAStarIFace()->GeneratePath(&GC.getAreaFinder(), pLoopPlot->getX(), pLoopPlot->getY(), -1, -1, pLoopPlot->isWater(), iArea);
		}
	}
}


void CvMap::toggleCitiesDisplay()
{
	PROFILE_EXTRA_FUNC();
	m_bCitiesDisplayed = !m_bCitiesDisplayed;

	gDLL->getInterfaceIFace()->clearSelectedCities();
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isAlive())
		{
			foreach_(CvCity* pCity, kPlayer.cities())
			{
				pCity->setVisible(m_bCitiesDisplayed);
			}
		}
	}
	AddDLLMessage(
		GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(),
		m_bCitiesDisplayed ? "City entities visible" : "City entities hidden",
		"AS2D_EXPOSED", MESSAGE_TYPE_INFO
	);
}

void CvMap::toggleUnitsDisplay()
{
	PROFILE_EXTRA_FUNC();
	int* paiFirstBuilt = new int[GC.getNumBuildingInfos()];
	int** paiBuiltNum = new int*[GC.getNumBuildingInfos()];
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		paiFirstBuilt[iI] = MAX_INT;
		paiBuiltNum[iI] = new int[8];
		for (int iJ = 0; iJ < 8; iJ++)
		{
			paiBuiltNum[iI][iJ] = 0;
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			//foreach_(CvUnit* pUnit, kPlayer.units())
			//{
			//	if (m_bUnitsDisplayed)
			//	{
			//		//gDLL->getEntityIFace()->RemoveUnitFromBattle(pUnit);
			//		//pUnit->removeEntity();
			//		//pUnit->destroyEntity();
			//		pUnit->kill(false);
			//	}
			//	else
			//	{
			//		//pUnit->createUnitEntity(pUnit);
			//		//pUnit->setupGraphical();
			//	}
			//}
			foreach_(const CvCity* pCity, kPlayer.cities())
			{
				std::map<BuildingTypes, BuiltBuildingData> ledger = pCity->getBuildingLedger();

				for (std::map<BuildingTypes, BuiltBuildingData>::const_iterator itr = ledger.begin(); itr != ledger.end(); ++itr)
				{
					const int iBuiltTime = itr->second.iTimeBuilt;
					if (iBuiltTime < paiFirstBuilt[itr->first])
					{
						paiFirstBuilt[itr->first] = iBuiltTime;
					}
					if (iBuiltTime < 0)
					{
						paiBuiltNum[itr->first][0]++;
					}
					else if (iBuiltTime < 200)
					{
						paiBuiltNum[itr->first][1]++;
					}
					else if (iBuiltTime < 400)
					{
						paiBuiltNum[itr->first][2]++;
					}
					else if (iBuiltTime < 600)
					{
						paiBuiltNum[itr->first][3]++;
					}
					else if (iBuiltTime < 800)
					{
						paiBuiltNum[itr->first][4]++;
					}
					else if (iBuiltTime < 1000)
					{
						paiBuiltNum[itr->first][5]++;
					}
					else if (iBuiltTime < 1200)
					{
						paiBuiltNum[itr->first][6]++;
					}
					else
					{
						paiBuiltNum[itr->first][7]++;
					}
				}
			}
		}
	}

	// Assemble table
	CvWString szTable;
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		/*if ((pabFirstBuilt[iI] > 400) && (pabFirstBuilt[iI] <= 600))
		{
			CvWString szBuffer;
			szBuffer.Format(L"%s : %d, %d ", GC.getBuildingInfo((BuildingTypes)iI).getText(), pabFirstBuilt[iI], GC.getGame().getBuildingCreatedCount((BuildingTypes)iI));
			szBuffer.append(CvWString(GC.getBuildingInfo((BuildingTypes)iI).getArtInfo()->getNIF()));
			AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
		}*/
		CvWString szBuffer;
		szBuffer.Format(L"%s,%d,%d,", GC.getBuildingInfo((BuildingTypes)iI).getText(), paiFirstBuilt[iI], GC.getGame().getBuildingCreatedCount((BuildingTypes)iI));
		szBuffer.append(CvWString(GC.getBuildingInfo((BuildingTypes)iI).getArtInfo()->getNIF()));
		szTable.append(szBuffer);
		szBuffer.clear();
		for (int iJ = 0; iJ < 8; iJ++)
		{
			szBuffer.Format(L",%d", paiBuiltNum[iI][iJ]);
			szTable.append(szBuffer);
		}
		szTable.append(NEWLINE);
	}

	CvString szTableOut;
	szTableOut.Convert(szTable);
	// Create a new file and write table to it
	std::ofstream fTable;
	fTable.open("BuildingsBuiltTable.csv");
	fTable << szTableOut.GetCString();
	fTable.close();

	delete[] paiFirstBuilt;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		delete[] paiBuiltNum[iI];
	}
	delete[] paiBuiltNum;

	m_bUnitsDisplayed = !m_bUnitsDisplayed;
	CvWString szBuffer = "Unit entities destroyed";
	if (m_bUnitsDisplayed)
		szBuffer = "Unit entities created";


	AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
}