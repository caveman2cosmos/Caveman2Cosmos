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

int CyArea::getUnitsPerPlayer(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea->getUnitsPerPlayer((PlayerTypes) eIndex);
}

int CyArea::getCitiesPerPlayer(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea->getCitiesPerPlayer((PlayerTypes) eIndex);
}

int CyArea::getBuildingHappiness(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea->getBuildingHappiness((PlayerTypes) eIndex);
}

int CyArea::getPower(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea->getPower((PlayerTypes) eIndex);
}

int CyArea::getBestFoundValue(int /*PlayerTypes*/ eIndex) const
{
	return m_pArea->getBestFoundValue((PlayerTypes) eIndex);
}

bool CyArea::isCleanPower(int /*TeamTypes*/ eIndex) const
{
	return m_pArea->isCleanPower((TeamTypes)eIndex);
}

bool CyArea::isBorderObstacle(int /*TeamTypes*/ eIndex) const
{
	return m_pArea->isBorderObstacle((TeamTypes)eIndex);
}

int CyArea::getYieldRateModifier(int /*PlayerTypes*/ eIndex1, int /*YieldTypes*/ eIndex2) const
{
	return m_pArea->getYieldRateModifier((PlayerTypes)eIndex1, (YieldTypes) eIndex2);
}

int CyArea::getNumBonuses(int /*BonusTypes*/ eBonus) const
{
	return m_pArea->getNumBonuses((BonusTypes)eBonus);
}

int CyArea::getNumTotalBonuses() const
{
	return m_pArea->getNumTotalBonuses();
}

void CyArea::changeCleanPowerCount(int /*TeamTypes*/ eIndex, int iChange) const
{
    m_pArea->changeCleanPowerCount((TeamTypes)eIndex, iChange);
}  
