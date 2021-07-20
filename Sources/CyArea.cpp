#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CyArea.h"

//
// Python wrapper class for CvArea
//

CyArea::CyArea() : m_pArea(NULL)
{
	FErrorMsg("Error");
}

CyArea::CyArea(CvArea* pArea) : m_pArea(pArea)
{
	FAssert(pArea != NULL);
}

int CyArea::calculateTotalBestNatureYield() const
{
	return m_pArea->calculateTotalBestNatureYield();
}

int CyArea::countCoastalLand() const
{
	return m_pArea->countCoastalLand();
}

int CyArea::countNumUniqueBonusTypes() const
{
	return m_pArea->countNumUniqueBonusTypes();
}

int CyArea::getID() const
{
	return m_pArea->getID();
}

int CyArea::getNumTiles() const
{
	return m_pArea->getNumTiles();
}

bool CyArea::isLake() const
{
	return m_pArea->isLake();
}

int CyArea::getNumRiverEdges() const
{
	return m_pArea->getNumRiverEdges();
}

int CyArea::getNumCities() const
{
	return m_pArea->getNumCities();
}

int CyArea::getNumUnits() const
{
	return m_pArea->getNumUnits();
}

int CyArea::getTotalPopulation() const
{
	return m_pArea->getTotalPopulation();
}

int CyArea::getNumStartingPlots() const
{
	return m_pArea->getNumStartingPlots();
}

bool CyArea::isWater() const
{
	return m_pArea->isWater();
}

int CyArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	return m_pArea->getUnitsPerPlayer(eIndex);
}

int CyArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	return m_pArea->getCitiesPerPlayer(eIndex);
}

int CyArea::getBuildingHappiness(PlayerTypes eIndex) const
{
	return m_pArea->getBuildingHappiness(eIndex);
}

int CyArea::getPower(PlayerTypes eIndex) const
{
	return m_pArea->getPower(eIndex);
}

int CyArea::getBestFoundValue(PlayerTypes eIndex) const
{
	return m_pArea->getBestFoundValue(eIndex);
}

bool CyArea::isCleanPower(TeamTypes eIndex) const
{
	return m_pArea->isCleanPower(eIndex);
}

bool CyArea::isBorderObstacle(TeamTypes eIndex) const
{
	return m_pArea->isBorderObstacle(eIndex);
}

int CyArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	return m_pArea->getYieldRateModifier(eIndex1, eIndex2);
}

int CyArea::getNumBonuses(BonusTypes eBonus) const
{
	return m_pArea->getNumBonuses(eBonus);
}

int CyArea::getNumTotalBonuses() const
{
	return m_pArea->getNumTotalBonuses();
}

void CyArea::changeCleanPowerCount(TeamTypes eIndex, int iChange) const
{
    m_pArea->changeCleanPowerCount(eIndex, iChange);
}
