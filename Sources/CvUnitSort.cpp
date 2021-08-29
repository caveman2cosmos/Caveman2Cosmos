//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitSort.cpp
//
//  PURPOSE: Sorting classes for units
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayer.h"

bool UnitSortBase::isLesserUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2) const
{
	// To keep the strict weak ordering for sorting, the result of the comparison cannot just be inverted, equal must always be false
	if (m_bInvert)
		return getUnitValue(pPlayer, pCity, eUnit1) < getUnitValue(pPlayer, pCity, eUnit2);
	else
		return getUnitValue(pPlayer, pCity, eUnit1) > getUnitValue(pPlayer, pCity, eUnit2);
}

bool UnitSortBase::isInverse() const
{
	return m_bInvert;
}

bool UnitSortBase::setInverse(bool bInvert)
{
	const bool bChanged = bInvert != m_bInvert;
	m_bInvert = bInvert;
	return bChanged;
}

UnitSortBase::~UnitSortBase()
{
}

int UnitSortStrength::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return std::max(GC.getUnitInfo(eUnit).getCombat(), GC.getUnitInfo(eUnit).getAirCombat());
}

int UnitSortMove::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getMoves();
}

int UnitSortCollateral::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getCollateralDamage();
}

int UnitSortRange::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return std::max(std::max(GC.getUnitInfo(eUnit).getAirRange(), GC.getUnitInfo(eUnit).getNukeRange()), GC.getUnitInfo(eUnit).getDropRange());
}

int UnitSortBombard::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getBombardRate();
}

int UnitSortCargo::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getCargoSpace();
}

int UnitSortWithdrawal::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getWithdrawalProbability();
}

int UnitSortPower::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getPowerValue();
}

int UnitSortCost::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	if (pCity)
	{
		return pCity->getProductionNeeded(eUnit) - pCity->getUnitProduction(eUnit);
	}
	else
	{
		return pPlayer->getProductionNeeded(eUnit);
	}
}

// dummy
int UnitSortName::getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return 0;
}

// name sorting defaults to A first
bool UnitSortName::isLesserUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2) const
{
	if (m_bInvert)
		return wcscmp(GC.getUnitInfo(eUnit1).getDescription(), GC.getUnitInfo(eUnit2).getDescription()) > 0;
	else
		return wcscmp(GC.getUnitInfo(eUnit1).getDescription(), GC.getUnitInfo(eUnit2).getDescription()) < 0;
}

UnitSortList::UnitSortList(const CvPlayer *pPlayer, const CvCity *pCity)
{
	m_pPlayer = pPlayer;
	m_pCity = pCity;

	m_apUnitSort[UNIT_SORT_NAME] = new UnitSortName();
	m_apUnitSort[UNIT_SORT_COST] = new UnitSortCost(true);
	m_apUnitSort[UNIT_SORT_STRENGTH] = new UnitSortStrength();
	m_apUnitSort[UNIT_SORT_MOVE] = new UnitSortMove();
	m_apUnitSort[UNIT_SORT_COLLATERAL] = new UnitSortCollateral();
	m_apUnitSort[UNIT_SORT_RANGE] = new UnitSortRange();
	m_apUnitSort[UNIT_SORT_BOMBARD] = new UnitSortBombard();
	m_apUnitSort[UNIT_SORT_CARGO] = new UnitSortCargo();
	m_apUnitSort[UNIT_SORT_WITHDRAWAL] = new UnitSortWithdrawal();
	m_apUnitSort[UNIT_SORT_POWER] = new UnitSortPower();

	m_eActiveSort = UNIT_SORT_COST;
}

UnitSortList::~UnitSortList()
{
	for (int i = 0; i < NUM_UNIT_SORT; i++)
	{
		delete m_apUnitSort[i];
	}
}

UnitSortTypes UnitSortList::getActiveSort() const
{
	return m_eActiveSort;
}

bool UnitSortList::setActiveSort(UnitSortTypes eActiveSort)
{
	FASSERT_BOUNDS(0, NUM_UNIT_SORT, eActiveSort)
	const bool bChanged = m_eActiveSort != eActiveSort;
	m_eActiveSort = eActiveSort;
	return bChanged;
}

void UnitSortList::setCity(const CvCity *pCity)
{
	m_pCity = pCity;
}

void UnitSortList::setPlayer(const CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool UnitSortList::operator ()(UnitTypes eUnit1, UnitTypes eUnit2) const
{
	return m_apUnitSort[m_eActiveSort]->isLesserUnit(m_pPlayer, m_pCity, eUnit1, eUnit2);
}