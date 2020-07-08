#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CyArea.h"

//
// Python wrapper class for CvArea 
//

CyArea::CyArea() : m_pArea(NULL) {}

CyArea::CyArea(CvArea* pArea) : m_pArea(pArea) {}


int CyArea::calculateTotalBestNatureYield() const
{
	return m_pArea ? m_pArea->calculateTotalBestNatureYield() : -1;
}

int CyArea::countCoastalLand() const
{
	return m_pArea ? m_pArea->countCoastalLand() : -1;
}

int CyArea::countNumUniqueBonusTypes() const
{
	return m_pArea ? m_pArea->countNumUniqueBonusTypes() : -1;
}

int CyArea::getID() const
{
	return m_pArea ? m_pArea->getID() : -1;
}

int CyArea::getNumTiles() const
{
	return m_pArea ? m_pArea->getNumTiles() : -1;
}

bool CyArea::isLake() const
{
	return m_pArea ? m_pArea->isLake() : -1;
}

int CyArea::getNumRiverEdges() const
{
	return m_pArea ? m_pArea->getNumRiverEdges() : -1;
}

int CyArea::getNumCities() const
{
	return m_pArea ? m_pArea->getNumCities() : -1;
}

int CyArea::getNumUnits() const
{
	return m_pArea ? m_pArea->getNumUnits() : -1;
}

int CyArea::getTotalPopulation() const
{
	return m_pArea ? m_pArea->getTotalPopulation() : -1;
}

int CyArea::getNumStartingPlots() const
{
	return m_pArea ? m_pArea->getNumStartingPlots() : -1;
}

bool CyArea::isWater() const
{
	return m_pArea ? m_pArea->isWater() : false;
}

int CyArea::getUnitsPerPlayer(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->getUnitsPerPlayer((PlayerTypes) eIndex) : -1;
}

int CyArea::getCitiesPerPlayer(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->getCitiesPerPlayer((PlayerTypes) eIndex) : -1;
}

int CyArea::getBuildingHappiness(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->getBuildingHappiness((PlayerTypes) eIndex) : -1;
}

int CyArea::getPower(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->getPower((PlayerTypes) eIndex) : -1;
}

int CyArea::getBestFoundValue(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->getBestFoundValue((PlayerTypes) eIndex) : -1;
}

bool CyArea::isCleanPower(int /*TeamTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->isCleanPower((TeamTypes)eIndex) : false;
}

bool CyArea::isBorderObstacle(int /*TeamTypes*/ eIndex) const
{
	return m_pArea ? m_pArea->isBorderObstacle((TeamTypes)eIndex) : false;
}

int CyArea::getYieldRateModifier(int /*PlayerTypes*/ eIndex1, int /*YieldTypes*/ eIndex2) const
{
	return m_pArea ? m_pArea->getYieldRateModifier((PlayerTypes)eIndex1, (YieldTypes) eIndex2) : -1;
}

int CyArea::getNumBonuses(int /*BonusTypes*/ eBonus) const
{
	return m_pArea ? m_pArea->getNumBonuses((BonusTypes)eBonus) : -1;
}

int CyArea::getNumTotalBonuses() const
{
	return m_pArea ? m_pArea->getNumTotalBonuses() : -1;
}

void CyArea::changeCleanPowerCount(int /*TeamTypes*/ eIndex, int iChange) const
{
    if (m_pArea) m_pArea->changeCleanPowerCount((TeamTypes)eIndex, iChange);
}  
