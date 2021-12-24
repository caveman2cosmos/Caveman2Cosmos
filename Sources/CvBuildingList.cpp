//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingList.cpp
//
//  PURPOSE: Filter, group, sort and cache a building list for a city or player
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvBuildingList.h"
#include "CvBuildingSort.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"

CvBuildingList::CvBuildingList(CvPlayer* pPlayer, CvCity* pCity)
	: m_bFilteringValid(false)
	, m_bGroupingValid(false)
	, m_bSortingValid(false)
	, m_pCity(pCity)
	, m_pPlayer(pPlayer)
	, m_aaiGroupedBuildingList(NULL)
	, m_BuildingFilters(pPlayer, pCity)
	, m_BuildingGrouping(pPlayer, pCity)
	, m_BuildingSort(pPlayer, pCity)
	, m_eSelectedBuilding(NO_BUILDING)
	, m_eSelectedWonder(NO_BUILDING)
{
}

void CvBuildingList::init()
{
	m_BuildingFilters.init();
	m_BuildingSort.init();
}

void CvBuildingList::setPlayerToOwner()
{
	if (m_pCity && !m_pPlayer)
	{
		CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());
		m_pPlayer = pPlayer;
		m_BuildingFilters.setPlayer(pPlayer);
		m_BuildingGrouping.setPlayer(pPlayer);
		m_BuildingSort.setPlayer(pPlayer);
	}
}

void CvBuildingList::setInvalid()
{
	m_bFilteringValid = false;
	m_bGroupingValid = false;
	m_bSortingValid = false;
}

bool CvBuildingList::getFilterActive(BuildingFilterTypes eFilter) const
{
	return m_BuildingFilters.isFilterActive(eFilter);
}

void CvBuildingList::setFilterActive(BuildingFilterTypes eFilter, bool bActive)
{
	if (m_BuildingFilters.setFilterActive(eFilter, bActive))
	{
		m_bFilteringValid = false;
		m_bGroupingValid = false;
		m_bSortingValid = false;
	}
}

BuildingGroupingTypes CvBuildingList::getGroupingActive() const
{
	return m_BuildingGrouping.getActiveGrouping();
}

void CvBuildingList::setGroupingActive(BuildingGroupingTypes eGrouping)
{
	if (m_BuildingGrouping.setActiveGrouping(eGrouping))
	{
		m_bGroupingValid = false;
		m_bSortingValid = false;
	}
}

BuildingSortTypes CvBuildingList::getSortingActive() const
{
	return m_BuildingSort.getActiveSort();
}

void CvBuildingList::setSortingActive(BuildingSortTypes eSorting)
{
	if (m_BuildingSort.setActiveSort(eSorting))
	{
		m_bSortingValid = false;
	}
}

int CvBuildingList::getGroupNum()
{
	if (!m_bGroupingValid)
	{
		doGroup();
	}
	return m_aaiGroupedBuildingList.size();
}

int CvBuildingList::getNumInGroup(int iGroup)
{
	if (!m_bGroupingValid)
	{
		doGroup();
	}
	FASSERT_BOUNDS(0, m_aaiGroupedBuildingList.size(), iGroup);
	return m_aaiGroupedBuildingList[iGroup].size();
}

BuildingTypes CvBuildingList::getBuildingType(int iGroup, int iPos)
{
	if (!m_bSortingValid)
	{
		doSort();
	}
	FASSERT_BOUNDS(0, getGroupNum(), iGroup);
	FASSERT_BOUNDS(0, getNumInGroup(iGroup), iPos);
	return m_aaiGroupedBuildingList[iGroup][iPos];
}

void CvBuildingList::doFilter()
{
	m_aiBuildingList.clear();
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(i);
		if (m_BuildingFilters.isFiltered(eBuilding))
			m_aiBuildingList.push_back(eBuilding);
	}
	m_bFilteringValid = true;
}

void CvBuildingList::doGroup()
{
	if (!m_bFilteringValid)
		doFilter();

	m_aaiGroupedBuildingList.clear();

	const int iSize = static_cast<int>(m_aiBuildingList.size());
	std::multimap<int, BuildingTypes> mmap_Buildings;

	for (int i = 0; i < iSize; i++)
	{
		mmap_Buildings.insert(std::pair<int, BuildingTypes>(m_BuildingGrouping.getGroup(m_aiBuildingList[i]), m_aiBuildingList[i]));
	}

	int iLastKey = MIN_INT;
	for (std::multimap<int, BuildingTypes>::iterator it = mmap_Buildings.begin(); it != mmap_Buildings.end(); ++it)
	{
		if (it->first != iLastKey || iLastKey == MIN_INT)
		{
			iLastKey = it->first;
			m_aaiGroupedBuildingList.push_back(std::vector<BuildingTypes>());
		}
		m_aaiGroupedBuildingList.back().push_back(it->second);
	}
	m_bGroupingValid = true;
}

void CvBuildingList::doSort()
{
	if (!m_bGroupingValid)
		doGroup();

	std::auto_ptr<BuildingSortListWrapper> pWrapper(new BuildingSortListWrapper(&m_BuildingSort));
	for (GroupedBuildingVector::iterator itr = m_aaiGroupedBuildingList.begin(); itr != m_aaiGroupedBuildingList.end(); ++itr)
	{
		algo::stable_sort(*itr, *pWrapper);
		pWrapper->deleteCache();
	}
	m_bSortingValid = true;
}

int CvBuildingList::getBuildingSelectionRow()
{
	if (m_eSelectedBuilding != NO_BUILDING)
	{
		for (int i = 0; i < static_cast<int>(m_aaiGroupedBuildingList.size()); i++)
		{
			if (algo::any_of_equal(m_aaiGroupedBuildingList[i], m_eSelectedBuilding))
				return i;
		}
		m_eSelectedBuilding = NO_BUILDING;
	}
	// Find first normal building
	for (int i = 0; i < static_cast<int>(m_aaiGroupedBuildingList.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(m_aaiGroupedBuildingList[i].size()); j++)
		{
			if (!isLimitedWonder(static_cast<BuildingTypes>(m_aaiGroupedBuildingList[i][j])))
				return i;
		}
	}
	return 0;
}

int CvBuildingList::getWonderSelectionRow()
{
	if (m_eSelectedWonder != NO_BUILDING)
	{
		for (int i = 0; i < static_cast<int>(m_aaiGroupedBuildingList.size()); i++)
		{
			if (algo::any_of_equal(m_aaiGroupedBuildingList[i], m_eSelectedWonder))
				return i;
		}
		m_eSelectedWonder = NO_BUILDING;
	}
	// Find first wonder
	for (int i = 0; i < static_cast<int>(m_aaiGroupedBuildingList.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(m_aaiGroupedBuildingList[i].size()); j++)
		{
			if (!isLimitedWonder(static_cast<BuildingTypes>(m_aaiGroupedBuildingList[i][j])))
				return i;
		}
	}
	return 0;
}

void CvBuildingList::setSelectedBuilding(BuildingTypes eSelectedBuilding)
{
	m_eSelectedBuilding = eSelectedBuilding;
}

void CvBuildingList::setSelectedWonder(BuildingTypes eSelectedWonder)
{
	m_eSelectedWonder = eSelectedWonder;
}

BuildingTypes CvBuildingList::getSelectedBuilding() const
{
	return m_eSelectedBuilding;
}

BuildingTypes CvBuildingList::getSelectedWonder() const
{
	return m_eSelectedWonder;
}
