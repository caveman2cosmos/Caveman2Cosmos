//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitFilters.cpp
//
//  PURPOSE: Classes to filter units
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvGameCoreDLLUnDefNew.h"
#include "CvGameCoreDLLDefNew.h"

void UnitFilterBase::Activate()
{
	setActive(true);
}

void UnitFilterBase::Deactivate()
{
	setActive(false);
}

bool UnitFilterBase::isActive()
{
	return m_bActive;
}

bool UnitFilterBase::setActive(bool bActive)
{
	bool bChanged = m_bActive ^ bActive;
	m_bActive = bActive;
	return bChanged;
}

bool UnitFilterBase::isFiltered(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return !m_bActive || (m_bInvert ^ isFilteredUnit(pPlayer, pCity, eUnit));
}

UnitFilterBase::~UnitFilterBase()
{

}

bool UnitFilterCanBuild::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	if (pCity)
	{
		return pCity->canTrain(eUnit, false, m_bShowSomeUnconstructable);
	}
	return pPlayer->canTrain(eUnit, false, m_bShowSomeUnconstructable);
}

bool UnitFilterIsLimited::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return isLimitedUnit(eUnit);
}

bool UnitFilterIsCombat::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return (UnitCombatTypes)(GC.getUnitInfo(eUnit).getUnitCombatType()) == m_eCombat;
}

bool UnitFilterIsCombats::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	UnitCombatTypes eCombat = (UnitCombatTypes)(GC.getUnitInfo(eUnit).getUnitCombatType());
	return std::find(m_eCombats.begin(), m_eCombats.end(), eCombat) != m_eCombats.end();
}

void UnitFilterIsCombats::addCombat(UnitCombatTypes eCombat)
{
	m_eCombats.push_back(eCombat);
}

bool UnitFilterIsCombats::isEmpty()
{
	return m_eCombats.empty();
}

bool UnitFilterIsDomain::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return ((DomainTypes)GC.getUnitInfo(eUnit).getDomainType()) == m_eDomain;
}

bool UnitFilterIsDefense::isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return GC.getUnitInfo(eUnit).isOnlyDefensive();
}

UnitFilterList::UnitFilterList(CvPlayer *pPlayer, CvCity *pCity)
	: m_apUnitFilters()
	, m_pCity(pCity)
	, m_pPlayer(pPlayer)
	, m_bInit(false)
{
	if ((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER", true) >= 0)
	{
		init();
	}
}

UnitFilterList::~UnitFilterList()
{
	if(m_bInit)
	{
		for (int i = 0; i < NUM_UNIT_FILTERS; i++)
		{
			delete m_apUnitFilters[i];
		}
	}
}

void UnitFilterList::init()
{
	if (!m_bInit)
	{
		m_apUnitFilters[UNIT_FILTER_HIDE_BASIC_INVISIBLE] = new UnitFilterCanBuild(true);
		if (m_pCity)
			m_apUnitFilters[UNIT_FILTER_HIDE_BASIC_INVISIBLE]->Activate();
		m_apUnitFilters[UNIT_FILTER_HIDE_BUILDABLE] = new UnitFilterCanBuild(false, true);
		m_apUnitFilters[UNIT_FILTER_HIDE_UNBUILDABLE] = new UnitFilterCanBuild();
		m_apUnitFilters[UNIT_FILTER_HIDE_LIMITED] = new UnitFilterIsLimited(true);
		m_apUnitFilters[UNIT_FILTER_SHOW_LAND] = new UnitFilterIsDomain(DOMAIN_LAND);
		m_apUnitFilters[UNIT_FILTER_SHOW_AIR] = new UnitFilterIsDomain(DOMAIN_AIR);
		m_apUnitFilters[UNIT_FILTER_SHOW_WATER] = new UnitFilterIsDomain(DOMAIN_SEA);
		m_apUnitFilters[UNIT_FILTER_SHOW_WORKERS] = new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER"));
		UnitFilterIsCombats* pFilter = new UnitFilterIsCombats();
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_CIVILIAN"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SPY"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SETTLER"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MISSIONARY"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_TRADE"));
		pFilter->addCombat(NO_UNITCOMBAT);
		m_apUnitFilters[UNIT_FILTER_SHOW_CIVILIAN] = pFilter;
		m_apUnitFilters[UNIT_FILTER_SHOW_SIEGE] = new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SIEGE"));
		m_apUnitFilters[UNIT_FILTER_SHOW_MOUNTED] = new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED"));
		m_apUnitFilters[UNIT_FILTER_SHOW_HEROES] = new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_HERO"));
		pFilter = new UnitFilterIsCombats(true);
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_CIVILIAN"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SPY"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SETTLER"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MISSIONARY"));
		pFilter->addCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_TRADE"));
		pFilter->addCombat(NO_UNITCOMBAT);
		m_apUnitFilters[UNIT_FILTER_SHOW_MILITARY] = pFilter;
		m_apUnitFilters[UNIT_FILTER_SHOW_DEFENSE] = new UnitFilterIsDefense();
		m_apUnitFilters[UNIT_FILTER_SHOW_MISSIONARY] = new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MISSIONARY"));

		m_apUnitFilters[UNIT_FILTER_HIDE_UNBUILDABLE]->setActive(getBugOptionBOOL("RoMSettings__HideUntrainableUnits", false));
		
		m_bInit = true;
	}
}

int UnitFilterList::getNumFilters()
{
	return NUM_UNIT_FILTERS;
}

bool UnitFilterList::isFilterActive(UnitFilterTypes i)
{
	FAssertMsg(i < NUM_UNIT_FILTERS, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_apUnitFilters[i]->isActive();
}

void UnitFilterList::setCity(CvCity *pCity)
{
	m_pCity = pCity;
}

void UnitFilterList::setPlayer(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool UnitFilterList::setFilterActive(UnitFilterTypes i, bool bActive)
{
	FAssertMsg(i < NUM_UNIT_FILTERS, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_apUnitFilters[i]->setActive(bActive);
}

bool UnitFilterList::isFiltered(UnitTypes eUnit)
{
	for (int i = 0; i < NUM_UNIT_FILTERS; i++)
	{
		if (!m_apUnitFilters[i]->isFiltered(m_pPlayer, m_pCity, eUnit))
			return false;
	}
	return true;
}

void UnitFilterList::setFilterActiveAll(UnitFilterTypes eFilter, bool bActive)
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			kLoopPlayer.setUnitListFilterActive(eFilter, bActive);
			int iIter;
			for (CvCity* pCity = kLoopPlayer.firstCity(&iIter); NULL != pCity; pCity = kLoopPlayer.nextCity(&iIter))
			{
				pCity->setUnitListFilterActive(eFilter, bActive);
			}
		}
	}
}
