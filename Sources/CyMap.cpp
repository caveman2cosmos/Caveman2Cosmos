//
// Python wrapper class for CvMap 
// 

#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"

CyMap::CyMap() : m_pMap(NULL) { m_pMap = &GC.getMap(); }

CyMap::CyMap(CvMap* pMap) : m_pMap(pMap) {}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
CyMap& CyMap::operator=(CvMap& kMap)
{
	m_pMap = &kMap;
	return *this;
}

int	CyMap::getViewportWidth() const
{
	return GC.getCurrentViewport()->getGridWidth();
}

int	CyMap::getViewportHeight() const
{
	return GC.getCurrentViewport()->getGridHeight();
}

int	CyMap::getViewportXFromMapX(int iX) const
{
	return GC.getCurrentViewport()->getViewportXFromMapX(iX);
}

int	CyMap::getViewportYFromMapY(int iY) const
{
	return GC.getCurrentViewport()->getViewportYFromMapY(iY);
}
	
bool CyMap::isInViewport(int iX, int iY) const
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

void CyMap::resetRevealedPlots(int /*TeamTypes*/ eTeam)
{
	if (m_pMap) m_pMap->resetRevealedPlots((TeamTypes)eTeam);
}

void CyMap::setAllPlotTypes(int /*PlotTypes*/ ePlotType)
{
	if (m_pMap) m_pMap->setAllPlotTypes((PlotTypes) ePlotType);
}

void CyMap::verifyUnitValidPlot()
{
	if (m_pMap) m_pMap->verifyUnitValidPlot();
}

CyCity* CyMap::findCity(int iX, int iY, int /*PlayerTypes*/ eOwner, int /*TeamTypes*/ eTeam, bool bSameArea, bool bCoastalOnly, int /*TeamTypes*/ eTeamAtWarWith, int /*DirectionTypes*/ eDirection, CyCity* pSkipCity) const
{
	return m_pMap ? new CyCity(m_pMap->findCity(iX, iY, (PlayerTypes)eOwner, (TeamTypes)eTeam, bSameArea, bCoastalOnly, ((TeamTypes)eTeamAtWarWith), (DirectionTypes)eDirection, pSkipCity->getCity())) : NULL;
}

CyArea* CyMap::findBiggestArea(bool bWater) const
{
	return m_pMap ? new CyArea(m_pMap->findBiggestArea(bWater)) : NULL;
}

int CyMap::getMapFractalFlags() const
{
	return m_pMap ? m_pMap->getMapFractalFlags() : -1;
}

int CyMap::numPlots() const
{
	return m_pMap ? m_pMap->numPlots() : -1;
}

int CyMap::plotNum(int iX, int iY) const
{
	return m_pMap ? m_pMap->plotNum(iX, iY) : -1;
}

int CyMap::getGridWidth() const
{
	return m_pMap->getGridWidth();
}

int CyMap::getGridHeight() const
{
	return m_pMap->getGridHeight();
}

int CyMap::getLandPlots() const
{
	return m_pMap ? m_pMap->getLandPlots() : -1;
}

int CyMap::getOwnedPlots() const
{
	return m_pMap ? m_pMap->getOwnedPlots() : -1;
}

int CyMap::getTopLatitude() const
{
	return m_pMap ? m_pMap->getTopLatitude() : -1;
}

int CyMap::getBottomLatitude() const
{
	return m_pMap ? m_pMap->getBottomLatitude() : -1;
}

int CyMap::getNextRiverID() const
{
	return m_pMap ? m_pMap->getNextRiverID() : -1;
}

void CyMap::incrementNextRiverID()
{
	if (m_pMap) m_pMap->incrementNextRiverID();
}

bool CyMap::isWrapX() const
{
	return m_pMap ? m_pMap->isWrapX() : false;
}

bool CyMap::isWrapY() const
{
	return m_pMap ? m_pMap->isWrapY() : false;
}

std::wstring CyMap::getMapScriptName() const
{
	return GC.getInitCore().getMapScriptName().GetCString();
}

WorldSizeTypes CyMap::getWorldSize() const
{
	return m_pMap ? m_pMap->getWorldSize() : NO_WORLDSIZE;
}

ClimateTypes CyMap::getClimate() const
{
	return m_pMap ? m_pMap->getClimate() : NO_CLIMATE;
}

SeaLevelTypes CyMap::getSeaLevel() const
{
	return m_pMap ? m_pMap->getSeaLevel() : NO_SEALEVEL;
}

int CyMap::getNumCustomMapOptions() const
{
	return m_pMap ? m_pMap->getNumCustomMapOptions() : 0;
}

CustomMapOptionTypes CyMap::getCustomMapOption(int iOption) const
{
	return m_pMap ? m_pMap->getCustomMapOption(iOption) : NO_CUSTOM_MAPOPTION;
}

int CyMap::getNumBonuses(int /*BonusTypes*/ eIndex) const
{
	return m_pMap ? m_pMap->getNumBonuses((BonusTypes)eIndex) : -1;
}

CyPlot* CyMap::plotByIndex(int iIndex) const
{
	return m_pMap ? new CyPlot(m_pMap->plotByIndex(iIndex)) : NULL;
}

//
// static version
//
CyPlot* CyMap::sPlotByIndex(int iIndex) const
{
	static CyPlot plot;
	if (m_pMap)
	{
		plot.setPlot(m_pMap->plotByIndex(iIndex));
		return &plot;
	}
	return NULL;
}

CyPlot* CyMap::plot(int iX, int iY)  const
{
	return new CyPlot(m_pMap->plot(iX, iY));
}

//
// static version
//
CyPlot* CyMap::sPlot(int iX, int iY) const
{
	static CyPlot p;
	p.setPlot(m_pMap->plot(iX, iY));
	return &p;
}

int CyMap::getIndexAfterLastArea() const
{
	return m_pMap ? m_pMap->getIndexAfterLastArea() : -1;
}

int CyMap::getNumAreas() const
{
	return m_pMap ? m_pMap->getNumAreas() : -1;
}

int CyMap::getNumLandAreas() const
{
	return m_pMap ? m_pMap->getNumLandAreas() : -1;
}

CyArea* CyMap::getArea(int iID) const
{
	return m_pMap ? new CyArea(m_pMap->getArea(iID)) : NULL;
}

void CyMap::recalculateAreas()
{
	if (m_pMap) m_pMap->recalculateAreas();
}

void CyMap::rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions)
{
	if (m_pMap) m_pMap->rebuild(iGridW, iGridH, iTopLatitude, iBottomLatitude, bWrapX, bWrapY, eWorldSize, eClimate, eSeaLevel, iNumCustomMapOptions, aeCustomMapOptions);
}

void CyMap::updateMinimapColor()
{
	if (m_pMap) m_pMap->updateMinimapColor();
}

bool CyMap::generatePathForHypotheticalUnit(CyPlot *pFrom, CyPlot *pTo, int ePlayer, int eUnit, int iFlags, int iMaxTurns) const
{
	return m_pMap ? m_pMap->generatePathForHypotheticalUnit(pFrom->getPlot(), pTo->getPlot(), (PlayerTypes) ePlayer, (UnitTypes) eUnit, iFlags, iMaxTurns) : false;
}

int CyMap::getLastPathStepNum() const
{
	return m_pMap ? m_pMap->getLastPathStepNum() : 0;
}

CyPlot* CyMap::getLastPathPlotByIndex(int index) const
{
	return m_pMap ? new CyPlot(m_pMap->getLastPathPlotByIndex(index)) : NULL;
}
