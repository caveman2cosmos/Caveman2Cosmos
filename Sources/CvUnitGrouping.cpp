//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitGrouping.cpp
//
//  PURPOSE: Grouping classes for units
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

int UnitGroupingBase::getGroup(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	const int iInverse = m_bInvert ? -1 : 1;
	return iInverse * getGroupUnit(pPlayer, pCity, eUnit);
}

UnitGroupingBase::~UnitGroupingBase()
{
}

int UnitGroupingSingle::getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return 1;
}

int UnitGroupingCombat::getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return GC.getUnitInfo(eUnit).getUnitCombatType();
}

int UnitGroupingDomain::getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	return GC.getUnitInfo(eUnit).getDomainType();
}

int UnitGroupingFilters::getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit)
{
	int iSize = m_apFilters.size();
	for (int i = 0; i < iSize; i++)
		if (m_apFilters[i]->isFiltered(pPlayer, pCity, eUnit))
			return i;
	return iSize;
}

void UnitGroupingFilters::addGroupingFilter(UnitFilterBase *pFilter)
{
	m_apFilters.push_back(pFilter);
}

UnitGroupingFilters::~UnitGroupingFilters()
{
	for (unsigned int i = 0; i < m_apFilters.size(); i++)
		delete m_apFilters[i];
}

UnitGroupingList::UnitGroupingList(CvPlayer *pPlayer, CvCity *pCity)
	: m_apUnitGrouping()
	, m_pCity(pCity)
	, m_pPlayer(pPlayer)
	, m_eActiveGrouping(NO_UNIT_GROUPING)
	, m_bInit(false)
{
	if ((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER", true) >= 0)
	{
		init();
	}
}

UnitGroupingList::~UnitGroupingList()
{
	if (m_bInit)
	{
		for (int i = 0; i < NUM_UNIT_GROUPING; i++)
		{
			delete m_apUnitGrouping[i];
		}
	}
}

void UnitGroupingList::init()
{
	if (!m_bInit)
	{
		m_apUnitGrouping[UNIT_GROUPING_SINGLE] = new UnitGroupingSingle();
		m_apUnitGrouping[UNIT_GROUPING_COMBAT] = new UnitGroupingCombat();
		m_apUnitGrouping[UNIT_GROUPING_DOMAIN] = new UnitGroupingDomain();
		UnitGroupingFilters* pGrouping = new UnitGroupingFilters();
		pGrouping->addGroupingFilter(new UnitFilterIsCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_HERO")));
		pGrouping->addGroupingFilter(new UnitFilterIsLimited());
		m_apUnitGrouping[UNIT_GROUPING_HERO] = pGrouping;

		m_eActiveGrouping = UNIT_GROUPING_SINGLE;
		m_bInit = true;
	}
}

int UnitGroupingList::getNumGrouping()
{
	return NUM_UNIT_GROUPING;
}

UnitGroupingTypes UnitGroupingList::getActiveGrouping()
{
	return m_eActiveGrouping;
}

void UnitGroupingList::setCity(CvCity *pCity)
{
	m_pCity = pCity;
}

void UnitGroupingList::setPlayer(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool UnitGroupingList::setActiveGrouping(UnitGroupingTypes eActiveGrouping)
{
	FAssertMsg(eActiveGrouping < NUM_UNIT_GROUPING, "Index out of bounds");
	FAssertMsg(eActiveGrouping > -1, "Index out of bounds");
	const bool bChanged = m_eActiveGrouping != eActiveGrouping;
	m_eActiveGrouping = eActiveGrouping;
	return bChanged;
}

int UnitGroupingList::getGroup(UnitTypes eUnit)
{
	return m_apUnitGrouping[m_eActiveGrouping]->getGroup(m_pPlayer, m_pCity, eUnit);
}
