//
//	FILE:	 CvMap.cpp
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//


#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"
#include "CvFractal.h"
#include "CvPlayerAI.h"

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
#include <direct.h>			// for getcwd()
#include <stdlib.h>			// for MAX_PATH
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

// Public Functions...

CvMap::CvMap()
	: m_iGridWidth(0)
	, m_iGridHeight(0)
	, m_iLandPlots(0)
	, m_iOwnedPlots(0)
	, m_iTopLatitude(0)
	, m_iBottomLatitude(0)
	, m_iNextRiverID(0)
	, m_eType(NO_MAP)
	, m_iCurrentViewportIndex(0)
	, m_bWrapX(false)
	, m_bWrapY(false)
	, m_paiNumBonus(NULL)
	, m_paiNumBonusOnLand(NULL)
	, m_bCitiesDisplayed(true)
	, m_bUnitsDisplayed(true)
	, m_pMapPlots(NULL)
{
}

CvMap::CvMap(MapTypes eType) /* Parallel Maps */
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
	OutputDebugString("Calling constructor for Map: Start");

	CvMapInitData defaultMapData;
	reset(&defaultMapData);

	OutputDebugString("Calling constructor for Map: End");
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
	OutputDebugString("Initializing Map: Start");
	PROFILE("CvMap::init");
	gDLL->logMemState( CvString::format("CvMap::init begin - world size=%s, climate=%s, sealevel=%s, num custom options=%6", 
		GC.getWorldInfo(GC.getInitCore().getWorldSize()).getDescription(), 
		GC.getClimateInfo(GC.getInitCore().getClimate()).getDescription(), 
		GC.getSeaLevelInfo(GC.getInitCore().getSeaLevel()).getDescription(),
		GC.getInitCore().getNumCustomMapOptions()).c_str() );

	Cy::call_optional(gDLL->getPythonIFace()->getMapScriptModule(), "beforeInit");

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

	OutputDebugString("Initializing Map: End");
}


void CvMap::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumBonus);
	SAFE_DELETE_ARRAY(m_paiNumBonusOnLand);

	SAFE_DELETE_ARRAY(m_pMapPlots);

	m_areas.uninit();

	for(int iI = 0; iI < (int)m_viewports.size(); iI++)
	{
		delete m_viewports[iI];
	}

	m_viewports.clear();
	m_iCurrentViewportIndex = -1;
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMap::reset(CvMapInitData* pInitInfo)
{
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
			if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getGridSize", Cy::Args() << GC.getInitCore().getWorldSize(), out)
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
		if(Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getTopLatitude", resultTop)
			&& Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getBottomLatitude", resultBottom))
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
		if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getWrapX", resultX)
			&& Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "getWrapY", resultY))
		{
			m_bWrapX = (resultX != 0);
			m_bWrapY = (resultY != 0);
		}
	}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	//Koshling - why do we ignore the map size in MapInfos if there is only 1???  Changed that for now
	//if (GC.getNumMapInfos() > 1)
	if (GC.multiMapsEnabled() /*&& GC.getMapInfos().size() > 0*/)
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
		FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvMap::reset");
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
	setCurrentViewport(addViewport(-1, -1, false));

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


// XXX generalize these funcs? (macro?)
void CvMap::doTurn()
{
	MEMORY_TRACE_FUNCTION();
	PROFILE("CvMap::doTurn()")

	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->doTurn();
	}
}


void CvMap::updateFlagSymbols()
{
	updateFlagSymbolsInternal(false);
}

void CvMap::updateFlagSymbolsInternal(bool bForce)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotByIndex(iI);

		if (bForce || pLoopPlot->isFlagDirty())
		{
			pLoopPlot->updateFlagSymbol();
			pLoopPlot->setFlagDirty(false);
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
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateVisibility();
	}
}


void CvMap::updateSymbolVisibility()
{
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
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateIrrigated();
	}
}


void CvMap::updateCenterUnit()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateCenterUnit();
	}
}


void CvMap::updateWorkingCity()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateWorkingCity();
	}
}


void CvMap::updateMinOriginalStartDist(CvArea* pArea)
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

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

		if (pStartingPlot != NULL && pStartingPlot->area() == pArea)
		{
			for (int iJ = 0; iJ < numPlots(); iJ++)
			{
				CvPlot* pLoopPlot = plotByIndex(iJ);

				if (pLoopPlot->area() == pArea)
				{
					//iDist = GC.getMap().calculatePathDistance(pStartingPlot, pLoopPlot);
					int iDist = stepDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

					if (iDist != -1)
					{
					    //int iCrowDistance = plotDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
					    //iDist = std::min(iDist,  iCrowDistance * 2);
						if ((pLoopPlot->getMinOriginalStartDist() == -1) || (iDist < pLoopPlot->getMinOriginalStartDist()))
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
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->updateYield();
	}
}


void CvMap::verifyUnitValidPlot()
{
	for (int iI = 0; iI < numPlots(); iI++)
	{
		plotByIndex(iI)->verifyUnitValidPlot();
	}
}


void CvMap::combinePlotGroups(PlayerTypes ePlayer, CvPlotGroup* pPlotGroup1, CvPlotGroup* pPlotGroup2, bool bRecalculateBonuses)
{
	CvPlotGroup* pNewPlotGroup;
	CvPlotGroup* pOldPlotGroup;

	FAssertMsg(pPlotGroup1 != NULL, "pPlotGroup is not assigned to a valid value");
	FAssertMsg(pPlotGroup2 != NULL, "pPlotGroup is not assigned to a valid value");

	if (pPlotGroup1 == pPlotGroup2)
	{
		return;
	}

	if (pPlotGroup1->getLengthPlots() > pPlotGroup2->getLengthPlots())
	{
		pNewPlotGroup = pPlotGroup1;
		pOldPlotGroup = pPlotGroup2;
	}
	else
	{
		pNewPlotGroup = pPlotGroup2;
		pOldPlotGroup = pPlotGroup1;
	}

	pNewPlotGroup->mergeIn(pOldPlotGroup, bRecalculateBonuses);
}

CvPlot* CvMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	CvPlot* pPlot = NULL;
	int iCount = 0;

	while (iCount < iTimeout)
	{
/*************************************************************************************************/
/**	Xienwolf Tweak							12/13/08											**/
/**																								**/
/**					Reduction in massive Random Spam in Logger files by using Map				**/
/*************************************************************************************************/
/**								---- Start Original Code ----									**
		pTestPlot = plotSorenINLINE(GC.getGame().getSorenRandNum(getGridWidth(), "Rand Plot Width"), GC.getGame().getSorenRandNum(getGridHeight(), "Rand Plot Height"));
/**								----  End Original Code  ----									**/
		CvPlot* pTestPlot = plotSorenINLINE(GC.getGame().getMapRandNum(getGridWidth(), "Rand Plot Width"), GC.getGame().getMapRandNum(getGridHeight(), "Rand Plot Height"));
/*************************************************************************************************/
/**	Tweak									END													**/
/*************************************************************************************************/

		FAssertMsg(pTestPlot != NULL, "TestPlot is not assigned a valid value");

		if ((iArea == -1) || (pTestPlot->getArea() == iArea))
		{
			bool bValid = true;

			if (iMinUnitDistance != -1)
			{
				for (int iDX = -(iMinUnitDistance); iDX <= iMinUnitDistance; iDX++)
				{
					for (int iDY = -(iMinUnitDistance); iDY <= iMinUnitDistance; iDY++)
					{
						const CvPlot* pLoopPlot = plotXY(pTestPlot->getX(), pTestPlot->getY(), iDX, iDY);

						if (pLoopPlot != NULL && pLoopPlot->isUnit())
						{
							bValid = false;
						}
					}
				}
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

	// XXX look for barbarian cities???

	int iBestValue = MAX_INT;
	CvCity* pBestCity = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (eOwner == NO_PLAYER || iI == eOwner)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kLoopPlayer.isAlive() && (eTeam == NO_TEAM || kLoopPlayer.getTeam() == eTeam))
			{
				for (CvPlayer::city_iterator cityItr = kLoopPlayer.beginCities(); cityItr != kLoopPlayer.endCities(); ++cityItr)
				{
					CvCity* pLoopCity = *cityItr;
					if (!bSameArea || pLoopCity->area() == plot(iX, iY)->area() || (bCoastalOnly && pLoopCity->waterArea() == plot(iX, iY)->area()))
					{
						if (!bCoastalOnly || pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
						{
							if (eTeamAtWarWith == NO_TEAM || atWar(kLoopPlayer.getTeam(), eTeamAtWarWith))
							{
								if (eDirection == NO_DIRECTION || estimateDirection(dxWrap(pLoopCity->getX() - iX), dyWrap(pLoopCity->getY() - iY)) == eDirection)
								{
									if (pSkipCity == NULL || pLoopCity != pSkipCity)
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
	// XXX look for barbarian cities???

	int iBestValue = MAX_INT;
	int iBestValueInViewport = MAX_INT;
	CvSelectionGroup* pBestSelectionGroup = NULL;
	CvSelectionGroup* pBestSelectionGroupInViewport = NULL;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && (eOwner == NO_PLAYER || iI == eOwner))
		{
			for (CvPlayer::group_iterator groupItr = GET_PLAYER((PlayerTypes)iI).beginGroups(); groupItr != GET_PLAYER((PlayerTypes)iI).endGroups(); ++groupItr)
			{
				CvSelectionGroup* pLoopGroup = *groupItr;
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
	PROFILE("CvMap::findWater()");

	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (int iDY = -(iRange); iDY <= iRange; iDY++)
		{
			const CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

			if (pLoopPlot != NULL && (bFreshWater ? pLoopPlot->isFreshWater() : pLoopPlot->isWater()))
			{
				return true;
			}
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
	m_iLandPlots = (m_iLandPlots + iChange);
	FAssert(getLandPlots() >= 0);
}


int CvMap::getOwnedPlots() const
{
	return m_iOwnedPlots;
}


void CvMap::changeOwnedPlots(int iChange)
{
	m_iOwnedPlots = (m_iOwnedPlots + iChange);
	FAssert(getOwnedPlots() >= 0);
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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumBonus[eIndex];
}


void CvMap::changeNumBonuses(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumBonus[eIndex] += iChange;
	FAssertMsg(m_paiNumBonus[eIndex] >= 0, "Negative bonus occurance on the map!");
}


int CvMap::getNumBonusesOnLand(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNumBonusOnLand[eIndex];
}


void CvMap::changeNumBonusesOnLand(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiNumBonusOnLand[eIndex] = (m_paiNumBonusOnLand[eIndex] + iChange);
	FAssert(getNumBonusesOnLand(eIndex) >= 0);
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
	return algo::count_if(GC.getMap().areas(), !CvArea::fn::isWater());
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
int CvMap::calculatePathDistance(CvPlot *pSource, CvPlot *pDest, CvPlot *pInvalidPlot)
// Super Forts end
{
	PROFILE_FUNC();

	if (pSource == NULL || pDest == NULL)
	{
		return -1;
	}

	// Super Forts begin *canal* *choke*
	// 1 must be added because 0 is already being used as the default value for iInfo in GeneratePath()
	int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : GC.getMap().plotNum(pInvalidPlot->getX(), pInvalidPlot->getY()) + 1;

	if (gDLL->getFAStarIFace()->GeneratePath(&GC.getStepFinder(), pSource->getX(), pSource->getY(), pDest->getX(), pDest->getY(), false, iInvalidPlot, true))
	// Super Forts end
	{
		FAStarNode* pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

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
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->setIsActivePlayerNoDangerCache(false);
			pLoopPlot->setIsActivePlayerHasDangerCache(false);
			pLoopPlot->CachePathValidityResult(NULL,false,false);
			pLoopPlot->CachePathValidityResult(NULL,true,false);
		}
	}
}


void CvMap::invalidateIsTeamBorderCache(TeamTypes eTeam)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot != NULL)
		{
			pLoopPlot->setIsTeamBorderCache(eTeam, false);
		}
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


//
// read object from a stream
// used during load
//
void CvMap::read(FDataStreamBase* pStream)
{
	OutputDebugString("Reading Map: Start");
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvMapInitData defaultMapData;

	// Init data before load
	reset(&defaultMapData);

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvMap", &uiFlag);	// flags for expansion

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

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated");
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

	WRAPPER_SKIP_ELEMENT(wrapper, "CvPlot", &g_plotTypeZobristHashes, SAVE_VALUE_TYPE_INT_ARRAY);

	// call the read of the free list CvArea class allocations
	ReadStreamableFFreeListTrashArray(m_areas, pStream);

	setup();

	WRAPPER_READ_OBJECT_END(wrapper);

	OutputDebugString("Reading Map: End");
}

// save object to a stream
// used during save
//
void CvMap::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	uint uiFlag=0;
	WRAPPER_WRITE(wrapper, "CvMap" ,uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvMap" ,m_iGridWidth);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iGridHeight);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iLandPlots);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iOwnedPlots);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iTopLatitude);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iBottomLatitude);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_iNextRiverID);

	WRAPPER_WRITE(wrapper, "CvMap" ,m_bWrapX);
	WRAPPER_WRITE(wrapper, "CvMap" ,m_bWrapY);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated");
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap" ,REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvMap" ,REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonusOnLand);

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

#ifdef THE_GREAT_WALL
	if ( GC.getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW )
	{
		GC.getGame().processGreatWall(false);
	}
#endif
	gDLL->getEngineIFace()->setResourceLayer(false);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			for (CvPlayer::unit_iterator unitItr = GET_PLAYER((PlayerTypes)i).beginUnits(); unitItr != GET_PLAYER((PlayerTypes)i).endUnits(); ++unitItr)
			{
				CvUnit* pLoopUnit = *unitItr;
				if ( !pLoopUnit->isUsingDummyEntities() )
				{
					if (gDLL->getEntityIFace()->IsSelected(pLoopUnit->getEntity()))
					{
						gDLL->getInterfaceIFace()->selectUnit(pLoopUnit, true, true);
					}
					if (GC.IsGraphicsInitialized())
					{
						gDLL->getEntityIFace()->RemoveUnitFromBattle(pLoopUnit);
						pLoopUnit->removeEntity();
					}
					pLoopUnit->destroyEntity();
				}
			}

			for (CvPlayer::city_iterator cityItr = GET_PLAYER((PlayerTypes)i).beginCities(); cityItr != GET_PLAYER((PlayerTypes)i).endCities(); ++cityItr)
			{
				CvCity* pLoopCity = *cityItr;
				if ( pLoopCity->getEntity() != NULL )
				{
					FAssert(pLoopCity->isInViewport());

					if (pLoopCity->isCitySelected())
					{
						gDLL->getInterfaceIFace()->clearSelectedCities();
					}
					pLoopCity->removeEntity();
					pLoopCity->destroyEntity();
				}
			}
		}
	}
	
	GC.clearSigns();

	for (i = 0; i < numPlots(); i++)
	{
		plotByIndex(i)->destroyGraphics();
	}
}

void CvMap::afterSwitch()
{
	PROFILE_FUNC();

	if (m_pMapPlots == NULL)		// if it hasn't been initialized yet...
	{
		if (GC.getMapInfo(getType()).getInitialWBMap().GetLength() > 0)
		{
			CyArgsList argsList;
			long lResult;

			char mapPath[1024];
			getcwd(mapPath, 1024);
			strcat(mapPath, GC.getMapInfo(getType()).getInitialWBMap().GetCString());

			argsList.add(mapPath);
			gDLL->getPythonIFace()->callFunction(PYWorldBuilderModule, "readAndApplyDesc", argsList.makeFunctionArgs(), &lResult);
			if (lResult < 0) // failed
			{
				AddDLLMessage((PlayerTypes)0, true, GC.getEVENT_MESSAGE_TIME(), L"Worldbuilder map failed to load");
			}
		}
		else if (GC.getMapInfo(getType()).getMapScript().GetLength() > 0)
		{
			init();
			CvMapGenerator& kGenerator = CvMapGenerator::GetInstance();
			kGenerator.setUseDefaultMapScript(false);
			kGenerator.generateRandomMap();
			kGenerator.addGameElements();
			kGenerator.setUseDefaultMapScript(true);
		}
		else
		{
			init();
			CvMapGenerator& kGenerator = CvMapGenerator::GetInstance();
			kGenerator.generateRandomMap();
			kGenerator.addGameElements();
		}
	}
	
	gDLL->getInterfaceIFace()->clearSelectionList();
	gDLL->getInterfaceIFace()->makeSelectionListDirty();

	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
	
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
	
	{
		PROFILE("CvMap::afterSwitch.RebuildAll");

		gDLL->getEngineIFace()->RebuildAllPlots();
	}

	for (i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			for (CvPlayer::city_iterator cityItr = GET_PLAYER((PlayerTypes)i).beginCities(); cityItr != GET_PLAYER((PlayerTypes)i).endCities(); ++cityItr)
			{
				CvCity* pLoopCity = *cityItr;
				//gDLL->getEntityIFace()->createCityEntity(pLoopCity);
				pLoopCity->setupGraphical();
			}
			
			for (CvPlayer::unit_iterator unitItr = GET_PLAYER((PlayerTypes)i).beginUnits(); unitItr != GET_PLAYER((PlayerTypes)i).endUnits(); ++unitItr)
			{
				CvUnit* pLoopUnit = *unitItr;
				if ( !pLoopUnit->isUsingDummyEntities() )
				{
					gDLL->getEntityIFace()->createUnitEntity(pLoopUnit);
					pLoopUnit->setupGraphical();
				}
			}
		}
	}

	setupGraphical();
	updateFog();
	updateSymbols();
	updateFlagSymbols();
	updateMinimapColor();

	// Reprocess landmarks and signs
	GC.reprocessSigns();

#ifdef THE_GREAT_WALL
	if ( GC.getCurrentViewport()->getTransformType() == VIEWPORT_TRANSFORM_TYPE_WINDOW )
	{
		GC.getGame().processGreatWall(true);
	}
#endif
	gDLL->getEngineIFace()->setResourceLayer(GC.getResourceLayer());
	gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
}

int	CvMap::getNumViewports() const
{
	return m_viewports.size();
}

CvViewport* CvMap::getViewport(int iIndex) const
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	return m_viewports[iIndex];
}

int CvMap::addViewport(int iXOffset, int iYOffset, bool bIsFullMapContext)	//	Returns new viewport index
{
	CvViewport*	viewport = new CvViewport(this, bIsFullMapContext);

	if ( iXOffset >= 0 && iYOffset >= 0 )
	{
		viewport->setMapOffset(iXOffset, iYOffset);
	}
	m_viewports.push_back(viewport);

	return (int)m_viewports.size()-1;
}

void CvMap::deleteViewport(int iIndex)
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	if (m_iCurrentViewportIndex == iIndex)
	{
		m_iCurrentViewportIndex = -1;
	}

	delete m_viewports[iIndex];

	for(unsigned int iI = iIndex; iI < m_viewports.size()-1; iI++)
	{
		m_viewports[iI] = m_viewports[iI+1];
	}

	m_viewports.pop_back();
}

void CvMap::setCurrentViewport(int iIndex)
{
	FAssert(iIndex >= 0 && iIndex < (int)m_viewports.size());

	m_iCurrentViewportIndex = iIndex;
}

CvViewport* CvMap::getCurrentViewport() const
{
	FAssert( m_iCurrentViewportIndex == -1 || m_iCurrentViewportIndex < (int)m_viewports.size() );

	return (m_iCurrentViewportIndex == -1 ? NULL : m_viewports[m_iCurrentViewportIndex]);
}
	
MapTypes CvMap::getType() const
{
	return m_eType;
}

void CvMap::setType(MapTypes eNewType)
{
	m_eType = eNewType;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/


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

			int iArea = pArea->getID();

			pLoopPlot->setArea(iArea);

			gDLL->getFAStarIFace()->GeneratePath(&GC.getAreaFinder(), pLoopPlot->getX(), pLoopPlot->getY(), -1, -1, pLoopPlot->isWater(), iArea);
		}
	}
}


// Private Functions...
/************************************************************************************************/
/* Afforess	                  Start		 07/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvMap::percentUnoccupiedLand(bool bExcludeWater, bool bIncludeBarbarian, bool bExcludePeaks, CvArea* pArea, int iRange, CvPlot* pRangeFromPlot)
{
	int iNumTiles = 0;
	int iNumTilesValid = 0;
	for (int iI = 0; iI < numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = plotByIndex(iI);
		if (!pLoopPlot->isWater() || !bExcludeWater)
		{
			if (pArea == NULL || pLoopPlot->area() == pArea)
			{
				if (!pLoopPlot->isPeak2(true) || !bExcludePeaks)
				{
					if ((iRange == -1 || pRangeFromPlot == NULL) || (plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pRangeFromPlot->getX(), pRangeFromPlot->getY()) <= iRange))
					{
						iNumTiles++;
						if (pLoopPlot->getOwner() == NO_PLAYER || (bIncludeBarbarian && pLoopPlot->isHominid()))
						{
							iNumTilesValid++;
						}
					}
				}
			}
		}
	}
	if (iNumTiles > 0)
	{
		GC.getGame().logMsg("%d Tiles were in %d Range, out of %d total in range tiles", iNumTilesValid, iRange, iNumTiles);
		return (iNumTilesValid * 100) / iNumTiles;
	}
	return 0;
}		
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

void CvMap::toggleCitiesDisplay()
{
	gDLL->getInterfaceIFace()->clearSelectedCities();
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			int iI = 0;
			int iLoop;
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				iI++;
				//if (iI > 1)
				//	break;

				if (m_bCitiesDisplayed)
				{
					//pCity->removeEntity();
					//pCity->destroyEntity();
					//pCity->plot()->setPlotCity(NULL);
					//CvWString szBuffer = "Destroying: ";
					//szBuffer.append(pCity->getName());
					//AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
					//pCity->killTestCheap();
					pCity->setVisible(false);
				}
				else
				{
					//pCity->createCityEntity(pCity);
					//pCity->setupGraphical();
					//pCity->plot()->setPlotCity(pCity);
					pCity->setVisible(true);
				}
			}
		}
	}
	//gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	m_bCitiesDisplayed = !m_bCitiesDisplayed;
	CvWString szBuffer = "City entities hidden";
	if (m_bCitiesDisplayed)
		szBuffer = "City entities visible";

	MEMORY_TRACK_EXEMPT();

	AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
}

void CvMap::toggleUnitsDisplay()
{
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
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			int iLoop;
			//for (CvUnit* pUnit = kPlayer.firstUnit(&iLoop); pUnit != NULL; pUnit = kPlayer.nextUnit(&iLoop))
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
			for (CvCity* pCity = kPlayer.firstCity(&iLoop); pCity != NULL; pCity = kPlayer.nextCity(&iLoop))
			{
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					if (pCity->getNumRealBuilding((BuildingTypes)iI) > 0)
					{
						int iBuiltTime = pCity->getBuildingOriginalTime((BuildingTypes)iI);
						if (iBuiltTime < paiFirstBuilt[iI])
						{
							paiFirstBuilt[iI] = pCity->getBuildingOriginalTime((BuildingTypes)iI);
						}
						if (iBuiltTime < 0)
						{
							paiBuiltNum[iI][0]++;
						}
						else if (iBuiltTime < 200)
						{
							paiBuiltNum[iI][1]++;
						}
						else if (iBuiltTime < 400)
						{
							paiBuiltNum[iI][2]++;
						}
						else if (iBuiltTime < 600)
						{
							paiBuiltNum[iI][3]++;
						}
						else if (iBuiltTime < 800)
						{
							paiBuiltNum[iI][4]++;
						}
						else if (iBuiltTime < 1000)
						{
							paiBuiltNum[iI][5]++;
						}
						else if (iBuiltTime < 1200)
						{
							paiBuiltNum[iI][6]++;
						}
						else
						{
							paiBuiltNum[iI][7]++;
						}

						//if (pCity->getBuildingOriginalTime((BuildingTypes)iI) > 1000)
						//{
						//	pCity->setNumRealBuilding((BuildingTypes)iI, 0);
						//}
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
			
	MEMORY_TRACK_EXEMPT();

	AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_INFO);
}

bool CvMap::generatePathForHypotheticalUnit(const CvPlot *pFrom, const CvPlot *pTo, PlayerTypes ePlayer, UnitTypes eUnit, int iFlags, int iMaxTurns)
{
	return CvSelectionGroup::getPathGenerator()->generatePathForHypotheticalUnit(pFrom, pTo, ePlayer, eUnit, iFlags, iMaxTurns);
}

CvPath& CvMap::getLastPath()
{
	return CvSelectionGroup::getPathGenerator()->getLastPath();
}

int CvMap::getLastPathStepNum()
{
	// length of the path is not the number of steps so we have to count
	CvPath::const_iterator it = getLastPath().begin();
	int i = 0;
	while (it.plot())
	{
		i++;
		++it;
	}
	return i;
}

CvPlot* CvMap::getLastPathPlotByIndex(int index)
{
	// we can only start from the beginning if we don't want to expose the iterator to Python
	CvPath::const_iterator it = getLastPath().begin();
	for (int i = 0; i<index; i++)
		++it;
	return it.plot();
}