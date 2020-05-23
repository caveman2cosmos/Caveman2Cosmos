//
// Python wrapper class for CvMap 
// 

#include "CvGameCoreDLL.h"
#include "CvInitCore.h"
#include "CvMapGenerator.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyMap.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"

CyMap::CyMap() : m_pMap(NULL)
{
	m_pMap = &GC.getMap();
}

CyMap::CyMap(CvMap* pMap) : m_pMap(pMap)
{
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

int CyMap::getType()
{
	return m_pMap ? m_pMap->getType() : NO_MAP;
}

void CyMap::setType(int iNewType)
{
	if (m_pMap)
		m_pMap->setType((MapTypes)iNewType);
}

CyMap& CyMap::operator=(CvMap& kMap)
{
	m_pMap = &kMap;
	return *this;
}

bool CyMap::viewportsEnabled()
{
	return GC.viewportsEnabled();
}

int	CyMap::getViewportWidth()
{
	return GC.getCurrentViewport()->getGridWidth();
}

int	CyMap::getViewportHeight()
{
	return GC.getCurrentViewport()->getGridHeight();
}

int	CyMap::getMapXFromViewportX(int iX)
{
	return GC.getCurrentViewport()->getMapXFromViewportX(iX);
}

int	CyMap::getMapYFromViewportY(int iY)
{
	return GC.getCurrentViewport()->getMapYFromViewportY(iY);
}

int	CyMap::getViewportXFromMapX(int iX)
{
	return GC.getCurrentViewport()->getViewportXFromMapX(iX);
}

int	CyMap::getViewportYFromMapY(int iY)
{
	return GC.getCurrentViewport()->getViewportYFromMapY(iY);
}
	
bool CyMap::isInViewport(int iX, int iY)
{
	return GC.getCurrentViewport()->isInViewport(iX, iY);
}

void CyMap::closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom)
{
	GC.getCurrentViewport()->closeAdvisor(advisorWidth, iMinimapLeft, iMinimapRight, iMinimapTop, iMinimapBottom);
}

void CyMap::bringIntoView(int iX, int iY, bool bLookAt, bool bForceCenter, bool bDisplayCityScreen, bool bSelectCity, bool bAddSelectedCity)
{
	GC.getCurrentViewport()->bringIntoView(iX, iY, NULL, bLookAt, bForceCenter, bDisplayCityScreen, bSelectCity, bAddSelectedCity);
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

void CyMap::erasePlots()
{
	if (m_pMap)
		m_pMap->erasePlots();
}

void CyMap::setRevealedPlots(int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly)
{
	if (m_pMap)
		m_pMap->setRevealedPlots((TeamTypes) eTeam, bNewValue, bTerrainOnly);
}

void CyMap::resetRevealedPlots(int /*TeamTypes*/ eTeam)
{
	if (m_pMap)
		m_pMap->resetRevealedPlots((TeamTypes)eTeam);
}

void CyMap::setAllPlotTypes(int /*PlotTypes*/ ePlotType)
{
	if (m_pMap)
		m_pMap->setAllPlotTypes((PlotTypes) ePlotType);
}

/************************************************************************************************/
/* REVOLUTION_MOD                         02/29/08                                jdog5000      */
/*                                                                                              */
/* Used by Barbarian civ                                                                        */
/************************************************************************************************/
void CyMap::verifyUnitValidPlot()
{
	if( m_pMap )
		m_pMap->verifyUnitValidPlot();
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
void CyMap::updateVisibility()
{
	if (m_pMap)
		m_pMap->updateVisibility();
}

CyPlot* CyMap::syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout)
{
	return m_pMap ? new CyPlot(m_pMap->syncRandPlot(iFlags, iArea, iMinUnitDistance, iTimeout)) : NULL;
}

CyCity* CyMap::findCity(int iX, int iY, int /*PlayerTypes*/ eOwner, int /*TeamTypes*/ eTeam, bool bSameArea, bool bCoastalOnly, int /*TeamTypes*/ eTeamAtWarWith, int /*DirectionTypes*/ eDirection, CyCity* pSkipCity)
{
	return m_pMap ? new CyCity(m_pMap->findCity(iX, iY, (PlayerTypes)eOwner, (TeamTypes)eTeam, bSameArea, bCoastalOnly, ((TeamTypes)eTeamAtWarWith), (DirectionTypes)eDirection, pSkipCity->getCity())) : NULL;
}

CySelectionGroup* CyMap::findSelectionGroup(int iX, int iY, int /*PlayerTypes*/ eOwner, bool bReadyToSelect, bool bWorkers)
{
	return m_pMap ? new CySelectionGroup(m_pMap->findSelectionGroup(iX, iY, (PlayerTypes)eOwner, bReadyToSelect, bWorkers)) : NULL;
}

CyArea* CyMap::findBiggestArea(bool bWater)
{
	return m_pMap ? new CyArea(m_pMap->findBiggestArea(bWater)) : NULL;
}

int CyMap::getMapFractalFlags()
{
	return m_pMap ? m_pMap->getMapFractalFlags() : -1;
}

bool CyMap::findWater(CyPlot* pPlot, int iRange, bool bFreshWater)
{
	return m_pMap ? m_pMap->findWater(pPlot->getPlot(), iRange, bFreshWater) : false;
}

bool CyMap::isPlot(int iX, int iY)
{
	return m_pMap ? m_pMap->isPlot(iX, iY) : false;
}

int CyMap::numPlots()
{
	return m_pMap ? m_pMap->numPlots() : -1;
}

int CyMap::plotNum(int iX, int iY)
{
	return m_pMap ? m_pMap->plotNum(iX, iY) : -1;
}

int CyMap::plotX(int iIndex)
{
	return m_pMap ? m_pMap->plotX(iIndex) : -1;
}

int CyMap::plotY(int iIndex)
{
	return m_pMap ? m_pMap->plotY(iIndex) : -1;
}

int CyMap::getGridWidth() 
{
	return m_pMap->getGridWidth();
}

int CyMap::getGridHeight()
{
	return m_pMap->getGridHeight();
}

int CyMap::getLandPlots()
{
	return m_pMap ? m_pMap->getLandPlots() : -1;
}

int CyMap::getOwnedPlots()
{
	return m_pMap ? m_pMap->getOwnedPlots() : -1;
}

int CyMap::getTopLatitude()
{
	return m_pMap ? m_pMap->getTopLatitude() : -1;
}

int CyMap::getBottomLatitude()
{
	return m_pMap ? m_pMap->getBottomLatitude() : -1;
}

int CyMap::getNextRiverID()
{
	return m_pMap ? m_pMap->getNextRiverID() : -1;
}

void CyMap::incrementNextRiverID()
{
	if (m_pMap)
		m_pMap->incrementNextRiverID();
}

bool CyMap::isWrapX()
{
	return m_pMap ? m_pMap->isWrapX() : false;
}

bool CyMap::isWrapY()
{
	return m_pMap ? m_pMap->isWrapY() : false;
}

std::wstring CyMap::getMapScriptName()
{
	return GC.getInitCore().getMapScriptName().GetCString();
}

WorldSizeTypes CyMap::getWorldSize()
{
	return m_pMap ? m_pMap->getWorldSize() : NO_WORLDSIZE;
}

ClimateTypes CyMap::getClimate()
{
	return m_pMap ? m_pMap->getClimate() : NO_CLIMATE;
}

SeaLevelTypes CyMap::getSeaLevel()
{
	return m_pMap ? m_pMap->getSeaLevel() : NO_SEALEVEL;
}

int CyMap::getNumCustomMapOptions()
{
	return m_pMap ? m_pMap->getNumCustomMapOptions() : 0;
}

CustomMapOptionTypes CyMap::getCustomMapOption(int iOption)
{
	return m_pMap ? m_pMap->getCustomMapOption(iOption) : NO_CUSTOM_MAPOPTION;
}

int CyMap::getNumBonuses(int /* BonusTypes */ eIndex)
{
	return m_pMap ? m_pMap->getNumBonuses((BonusTypes)eIndex) : -1;
}

int CyMap::getNumBonusesOnLand(int /* BonusTypes */ eIndex)
{
	return m_pMap ? m_pMap->getNumBonusesOnLand((BonusTypes)eIndex) : -1;
}

CyPlot* CyMap::plotByIndex(int iIndex)
{
	return m_pMap ? new CyPlot(m_pMap->plotByIndex(iIndex)) : NULL;
}

//
// static version
//
CyPlot* CyMap::sPlotByIndex(int iIndex)
{
	static CyPlot plot;
	if (m_pMap)
	{
		plot.setPlot(m_pMap->plotByIndex(iIndex));
		return &plot;
	}
	return NULL;
}

CyPlot* CyMap::plot(int iX, int iY) 
{
	return new CyPlot(m_pMap->plot(iX, iY));
}

//
// static version
//
CyPlot* CyMap::sPlot(int iX, int iY) 
{
	static CyPlot p;
	p.setPlot(m_pMap->plot(iX, iY));
	return &p;
}

CyPlot* CyMap::pointToPlot(float fX, float fY)
{
	return m_pMap ? new CyPlot(m_pMap->pointToPlot(fX, fY)) : NULL;
}

int CyMap::getIndexAfterLastArea()
{
	return m_pMap ? m_pMap->getIndexAfterLastArea() : -1;
}

int CyMap::getNumAreas()
{
	return m_pMap ? m_pMap->getNumAreas() : -1;
}

int CyMap::getNumLandAreas()
{
	return m_pMap ? m_pMap->getNumLandAreas() : -1;
}

CyArea* CyMap::getArea(int iID)
{
	return m_pMap ? new CyArea(m_pMap->getArea(iID)) : NULL;
}

void CyMap::recalculateAreas()
{
	if (m_pMap)
		m_pMap->recalculateAreas();
}

void CyMap::resetPathDistance()
{
	if (m_pMap)
		m_pMap->resetPathDistance();
}

int CyMap::calculatePathDistance(CyPlot* pSource, CyPlot* pDest)
{
	if (m_pMap)
		return m_pMap->calculatePathDistance(pSource->getPlot(), pDest->getPlot());
	return -1;
}

void CyMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	if (m_pMap)
	{
		m_pMap->rebuild(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY, eWorldSize, eClimate, eSeaLevel, iNumCustomMapOptions, aeCustomMapOptions);
	}
}

void CyMap::regenerateGameElements()
{
	if (m_pMap)
	{
		CvMapGenerator* pMapGen = &CvMapGenerator::GetInstance();
		pMapGen->eraseRivers();
		pMapGen->eraseFeatures();
		pMapGen->eraseBonuses();
		pMapGen->eraseGoodies();
		pMapGen->addGameElements();
	}
}

void CyMap::updateFog()
{
	if (m_pMap)
	{
		m_pMap->updateFog();
	}
}

void CyMap::updateMinimapColor()
{
	if (m_pMap)
	{
		m_pMap->updateMinimapColor();
	}
}

void CyMap::updateMinOriginalStartDist(CyArea* pArea)
{
	if (m_pMap)
	{
		m_pMap->updateMinOriginalStartDist(pArea->getArea());
	}
}


bool CyMap::generatePathForHypotheticalUnit(CyPlot *pFrom, CyPlot *pTo, int ePlayer, int eUnit, int iFlags, int iMaxTurns)
{
	return m_pMap ? m_pMap->generatePathForHypotheticalUnit(pFrom->getPlot(), pTo->getPlot(), (PlayerTypes) ePlayer, (UnitTypes) eUnit, iFlags, iMaxTurns) : false;
}

int CyMap::getLastPathStepNum()
{
	return m_pMap ? m_pMap->getLastPathStepNum() : 0;
}

CyPlot* CyMap::getLastPathPlotByIndex(int index)
{
	return m_pMap ? new CyPlot(m_pMap->getLastPathPlotByIndex(index)) : NULL;
}

/************************************************************************************************/
/* Afforess	                  Start		 07/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
python::tuple CyMap::firstArea(bool bRev)
{
	int iterIn = 0;
	CvArea* pvObj = m_pMap ? m_pMap->firstArea(&iterIn, bRev) : NULL;
	CyArea* pyObj = pvObj ? new CyArea(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}

// returns tuple of (CyArea, iterOut)
python::tuple CyMap::nextArea(int iterIn, bool bRev)
{
	CvArea* pvObj = m_pMap ? m_pMap->nextArea(&iterIn, bRev) : NULL;
	CyArea* pyObj = pvObj ? new CyArea(pvObj) : NULL;
	python::tuple tup=python::make_tuple(pyObj, iterIn);
	delete pyObj;
	return tup;
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

// Super Forts begin *canal* *choke*
void CyMap::calculateCanalAndChokePoints()
{
	if(m_pMap)
	{
		m_pMap->calculateCanalAndChokePoints();
	}
}
// Super Forts end