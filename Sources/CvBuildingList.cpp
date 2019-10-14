//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingList.cpp
//
//  PURPOSE: Filter, group, sort and cache a building list for a city or player
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

CvBuildingList::CvBuildingList(CvPlayer* pPlayer, CvCity* pCity)
	: m_bFilteringValid(false), m_bGroupingValid(false), m_bSortingValid(false), m_pCity(pCity), m_pPlayer(pPlayer), m_aaiGroupedBuildingList(NULL), m_BuildingFilters(pPlayer, pCity), m_BuildingGrouping(pPlayer, pCity), m_BuildingSort(pPlayer, pCity), m_eSelectedBuilding(NO_BUILDING), m_eSelectedWonder(NO_BUILDING)
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
		CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwnerINLINE());
		m_pPlayer		  = pPlayer;
		m_BuildingFilters.setPlayer(pPlayer);
		m_BuildingGrouping.setPlayer(pPlayer);
		m_BuildingSort.setPlayer(pPlayer);
	}
}

void CvBuildingList::setInvalid()
{
	m_bFilteringValid = false;
	m_bGroupingValid  = false;
	m_bSortingValid	  = false;
}

bool CvBuildingList::getFilterActive(BuildingFilterTypes eFilter)
{
	return m_BuildingFilters.isFilterActive(eFilter);
}

void CvBuildingList::setFilterActive(BuildingFilterTypes eFilter, bool bActive)
{
	if (m_BuildingFilters.setFilterActive(eFilter, bActive))
	{
		m_bFilteringValid = false;
		m_bGroupingValid  = false;
		m_bSortingValid	  = false;
	}
}

BuildingGroupingTypes CvBuildingList::getGroupingActive()
{
	return m_BuildingGrouping.getActiveGrouping();
}

void CvBuildingList::setGroupingActive(BuildingGroupingTypes eGrouping)
{
	if (m_BuildingGrouping.setActiveGrouping(eGrouping))
	{
		m_bGroupingValid = false;
		m_bSortingValid	 = false;
	}
}

BuildingSortTypes CvBuildingList::getSortingActive()
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
	FAssertMsg(iGroup < (int)m_aaiGroupedBuildingList.size(), "Index out of bounds");
	FAssertMsg(iGroup > -1, "Index out of bounds");
	return m_aaiGroupedBuildingList[iGroup]->size();
}

BuildingTypes CvBuildingList::getBuildingType(int iGroup, int iPos)
{
	if (!m_bSortingValid)
	{
		doSort();
	}
	FAssertMsg(iGroup < getGroupNum(), "Index out of bounds");
	FAssertMsg(iGroup > -1, "Index out of bounds");
	FAssertMsg(iPos < getNumInGroup(iGroup), "Index out of bounds");
	FAssertMsg(iPos > -1, "Index out of bounds");
	return (*m_aaiGroupedBuildingList[iGroup])[iPos];
}

void CvBuildingList::doFilter()
{
	m_aiBuildingList.clear();
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		BuildingTypes eBuilding = (BuildingTypes)i;
		if (m_BuildingFilters.isFiltered(eBuilding))
			m_aiBuildingList.push_back(eBuilding);
	}
	m_bFilteringValid = true;
}

void CvBuildingList::doGroup()
{
	if (!m_bFilteringValid)
		doFilter();

	for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
	{
		delete m_aaiGroupedBuildingList[i];
	}
	m_aaiGroupedBuildingList.clear();

	int								  iSize = m_aiBuildingList.size();
	std::multimap<int, BuildingTypes> mmap_Buildings;

	for (int i = 0; i < iSize; i++)
		mmap_Buildings.insert(std::pair<int, BuildingTypes>(m_BuildingGrouping.getGroup(m_aiBuildingList[i]), m_aiBuildingList[i]));

	int index	 = -1;
	int iLastKey = MIN_INT;
	for (std::multimap<int, BuildingTypes>::iterator it = mmap_Buildings.begin(); it != mmap_Buildings.end(); ++it)
	{
		if (it->first != iLastKey)
		{
			iLastKey = it->first;
			index++;
			m_aaiGroupedBuildingList.push_back(new std::vector<BuildingTypes>);
		}
		m_aaiGroupedBuildingList[index]->push_back(it->second);
	}
	m_bGroupingValid = true;
}

void CvBuildingList::doSort()
{
	if (!m_bGroupingValid)
		doGroup();

	BuildingSortListWrapper* pWrapper = new BuildingSortListWrapper(&m_BuildingSort);
	for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
	{
		std::stable_sort(m_aaiGroupedBuildingList[i]->begin(), m_aaiGroupedBuildingList[i]->end(), *pWrapper);
		pWrapper->deleteCache();
	}
	delete pWrapper;
	m_bSortingValid = true;
}

int CvBuildingList::getBuildingSelectionRow()
{
	if (m_eSelectedBuilding != NO_BUILDING)
	{
		for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
		{
			if (std::find(m_aaiGroupedBuildingList[i]->begin(), m_aaiGroupedBuildingList[i]->end(), m_eSelectedBuilding) != m_aaiGroupedBuildingList[i]->end())
				return i;
		}
		m_eSelectedBuilding = NO_BUILDING;
	}
	// Find first normal building
	for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
	{
		for (unsigned int j = 0; j < m_aaiGroupedBuildingList[i]->size(); j++)
		{
			BuildingTypes eBuilding = (*m_aaiGroupedBuildingList[i])[j];
			if (!isLimitedWonderClass((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()))
				return i;
		}
	}
	return 0;
}

int CvBuildingList::getWonderSelectionRow()
{
	if (m_eSelectedWonder != NO_BUILDING)
	{
		for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
		{
			if (std::find(m_aaiGroupedBuildingList[i]->begin(), m_aaiGroupedBuildingList[i]->end(), m_eSelectedWonder) != m_aaiGroupedBuildingList[i]->end())
				return i;
		}
		m_eSelectedWonder = NO_BUILDING;
	}
	// Find first wonder
	for (unsigned int i = 0; i < m_aaiGroupedBuildingList.size(); i++)
	{
		for (unsigned int j = 0; j < m_aaiGroupedBuildingList[i]->size(); j++)
		{
			BuildingTypes eBuilding = (*m_aaiGroupedBuildingList[i])[j];
			if (isLimitedWonderClass((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()))
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

BuildingTypes CvBuildingList::getSelectedBuilding()
{
	return m_eSelectedBuilding;
}

BuildingTypes CvBuildingList::getSelectedWonder()
{
	return m_eSelectedWonder;
}
