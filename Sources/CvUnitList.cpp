//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitList.cpp
//
//  PURPOSE: Filter, group, sort and cache a unit list for a city or player
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"

CvUnitList::CvUnitList(CvPlayer* pPlayer, CvCity* pCity) :
m_bFilteringValid(false),
m_bGroupingValid(false),
m_bSortingValid(false),
m_pCity(pCity),
m_pPlayer(pPlayer),
m_aaiGroupedUnitList(NULL),
m_UnitFilters(pPlayer, pCity),
m_UnitGrouping(pPlayer, pCity),
m_UnitSort(pPlayer, pCity),
m_eSelectedUnit(NO_UNIT)
{
}

void CvUnitList::init()
{
	if ((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WORKER", true) >= 0)
	{
		m_UnitFilters.init();
		m_UnitGrouping.init();
	}
}

void CvUnitList::setPlayerToOwner()
{
	if (m_pCity && !m_pPlayer)
	{
		CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());
		m_pPlayer = pPlayer;
		m_UnitFilters.setPlayer(pPlayer);
		m_UnitGrouping.setPlayer(pPlayer);
		m_UnitSort.setPlayer(pPlayer);
	}
}

void CvUnitList::setInvalid()
{
	m_bFilteringValid = false;
	m_bGroupingValid = false;
	m_bSortingValid = false;
}

bool CvUnitList::getFilterActive(UnitFilterTypes eFilter) const
{
	return m_UnitFilters.isFilterActive(eFilter);
}

void CvUnitList::setFilterActive(UnitFilterTypes eFilter, bool bActive)
{
	if (m_UnitFilters.setFilterActive(eFilter, bActive))
	{
		m_bFilteringValid = false;
		m_bGroupingValid = false;
		m_bSortingValid = false;
	}
}

UnitGroupingTypes CvUnitList::getGroupingActive() const
{
	return m_UnitGrouping.getActiveGrouping();
}

void CvUnitList::setGroupingActive(UnitGroupingTypes eGrouping)
{
	if (m_UnitGrouping.setActiveGrouping(eGrouping))
	{
		m_bGroupingValid = false;
		m_bSortingValid = false;
	}
}

UnitSortTypes CvUnitList::getSortingActive() const
{
	return m_UnitSort.getActiveSort();
}

void CvUnitList::setSortingActive(UnitSortTypes eSorting)
{
	if (m_UnitSort.setActiveSort(eSorting))
	{
		m_bSortingValid = false;
	}
}

int CvUnitList::getGroupNum()
{
	if (!m_bGroupingValid)
	{
		doGroup();
	}
	return m_aaiGroupedUnitList.size();
}

int CvUnitList::getNumInGroup(int iGroup)
{
	if (!m_bGroupingValid)
	{
		doGroup();
	}
	FASSERT_BOUNDS(0, (int)m_aaiGroupedUnitList.size(), iGroup)
	return m_aaiGroupedUnitList[iGroup]->size();
}

UnitTypes CvUnitList::getUnitType(int iGroup, int iPos)
{
	if (!m_bSortingValid)
	{
		doSort();
	}
	FASSERT_BOUNDS(0, getGroupNum(), iGroup)
	FASSERT_BOUNDS(0, getNumInGroup(iGroup), iPos)
	return (*m_aaiGroupedUnitList[iGroup])[iPos];
}

void CvUnitList::doFilter()
{
	m_aiUnitList.clear();
	for (int i = 0; i < GC.getNumUnitInfos(); i++)
	{
		const UnitTypes eUnit = (UnitTypes) i;
		if (m_UnitFilters.isFiltered(eUnit))
			m_aiUnitList.push_back(eUnit);
	}
	m_bFilteringValid = true;
}

void CvUnitList::doGroup()
{
	if (!m_bFilteringValid)
		doFilter();

	for(unsigned int i=0; i<m_aaiGroupedUnitList.size(); i++)
	{
		delete m_aaiGroupedUnitList[i];
	}
	m_aaiGroupedUnitList.clear();

	const int iSize = m_aiUnitList.size();
	std::multimap<int, UnitTypes> mmap_Units;

	for (int i=0; i < iSize; i++)
		mmap_Units.insert(std::pair<int,UnitTypes>(m_UnitGrouping.getGroup(m_aiUnitList[i]), m_aiUnitList[i]));

	int index = -1;
	int iLastKey = MIN_INT;
	for (std::multimap<int, UnitTypes>::iterator it = mmap_Units.begin(); it != mmap_Units.end(); ++it)
	{
		if (it->first != iLastKey || index == -1)
		{
			iLastKey = it->first;
			index++;
			m_aaiGroupedUnitList.push_back(new std::vector<UnitTypes>);
		}
		m_aaiGroupedUnitList[index]->push_back(it->second);
	}
	m_bGroupingValid = true;
}

void CvUnitList::doSort()
{
	if (!m_bGroupingValid)
		doGroup();

	UnitSortListWrapper* pWrapper = new UnitSortListWrapper(&m_UnitSort);
	for (unsigned int i=0; i<m_aaiGroupedUnitList.size(); i++)
	{
		std::stable_sort(m_aaiGroupedUnitList[i]->begin(), m_aaiGroupedUnitList[i]->end(), *pWrapper);
	}
	delete pWrapper;
	m_bSortingValid = true;
}

int CvUnitList::getSelectionRow()
{
	if (m_eSelectedUnit == NO_UNIT)
		return 0;

	for (unsigned int i=0; i<m_aaiGroupedUnitList.size(); i++)
	{
		if (std::find(m_aaiGroupedUnitList[i]->begin(), m_aaiGroupedUnitList[i]->end(), m_eSelectedUnit) != m_aaiGroupedUnitList[i]->end())
			return i;
	}

	m_eSelectedUnit = NO_UNIT;
	return 0;
}

void CvUnitList::setSelectedUnit(UnitTypes eSelectedUnit)
{
	m_eSelectedUnit = eSelectedUnit;
}

UnitTypes CvUnitList::getSelectedUnit() const
{
	return m_eSelectedUnit;
}


