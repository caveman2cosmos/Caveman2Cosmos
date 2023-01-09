//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitGrouping.cpp
//
//  PURPOSE: Grouping classes for units
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayer.h"
#include "CvInfos.h"

int UnitGroupingBase::getGroup(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	const int iInverse = m_bInvert ? -1 : 1;
	return iInverse * getGroupUnit(pPlayer, pCity, eUnit);
}

UnitGroupingBase::~UnitGroupingBase()
{
}

int UnitGroupingSingle::getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return 1;
}

int UnitGroupingCombat::getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getUnitCombatType();
}

int UnitGroupingDomain::getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getDomainType();
}

int UnitGroupingFilters::getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const
{
	const int iSize = m_apFilters.size();
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

UnitGroupingList::UnitGroupingList(const CvPlayer *pPlayer, const CvCity *pCity)
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
		pGrouping->addGroupingFilter(new UnitFilterIsCombat(GC.getUNITCOMBAT_HERO()));
		pGrouping->addGroupingFilter(new UnitFilterIsLimited());
		m_apUnitGrouping[UNIT_GROUPING_HERO] = pGrouping;

		m_eActiveGrouping = UNIT_GROUPING_SINGLE;
		m_bInit = true;
	}
}

UnitGroupingTypes UnitGroupingList::getActiveGrouping() const
{
	return m_eActiveGrouping;
}

void UnitGroupingList::setCity(const CvCity *pCity)
{
	m_pCity = pCity;
}

void UnitGroupingList::setPlayer(const CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool UnitGroupingList::setActiveGrouping(UnitGroupingTypes eActiveGrouping)
{
	FASSERT_BOUNDS(0, NUM_UNIT_GROUPING, eActiveGrouping);
	const bool bChanged = m_eActiveGrouping != eActiveGrouping;
	m_eActiveGrouping = eActiveGrouping;
	return bChanged;
}

int UnitGroupingList::getGroup(UnitTypes eUnit) const
{
	return m_apUnitGrouping[m_eActiveGrouping]->getGroup(m_pPlayer, m_pCity, eUnit);
}
