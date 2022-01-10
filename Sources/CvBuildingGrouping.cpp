//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingGrouping.cpp
//
//  PURPOSE: Grouping classes for buildings
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvBuildingGrouping.h"

int BuildingGroupingBase::getGroup(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	const int iInverse = m_bInvert ? -1 : 1;
	return iInverse * getGroupBuilding(pPlayer, pCity, eBuilding);
}

BuildingGroupingBase::~BuildingGroupingBase()
{
}

int BuildingGroupingSingle::getGroupBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	return 1;
}

int BuildingGroupingWonderType::getGroupBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	if (!isLimitedWonder(eBuilding))
		return 0;
	if (isNationalWonder(eBuilding))
		return 1;
	if (isWorldWonder(eBuilding))
		return 3;
	return 2;
}

int BuildingGroupingFilters::getGroupBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const
{
	const int iSize = m_apFilters.size();
	for (int i = 0; i < iSize; i++)
		if (m_apFilters[i]->isFiltered(pPlayer, pCity, eBuilding))
			return i;
	return iSize;
}

void BuildingGroupingFilters::addGroupingFilter(BuildingFilterBase *pFilter)
{
	m_apFilters.push_back(pFilter);
}

BuildingGroupingFilters::~BuildingGroupingFilters()
{
	for (unsigned int i = 0; i < m_apFilters.size(); i++)
		delete m_apFilters[i];
}

BuildingGroupingList::BuildingGroupingList(CvPlayer *pPlayer, CvCity *pCity) : m_pPlayer(pPlayer), m_pCity(pCity)
{
	m_apBuildingGrouping[BUILDING_GROUPING_SINGLE] = new BuildingGroupingSingle();
	m_apBuildingGrouping[BUILDING_GROUPING_WONDER_TYPE] = new BuildingGroupingWonderType();
	BuildingGroupingFilters* pGrouping = new BuildingGroupingFilters();
	pGrouping->addGroupingFilter(new BuildingFilterIsMilitary());
	pGrouping->addGroupingFilter(new BuildingFilterIsCityDefense());
	m_apBuildingGrouping[BUILDING_GROUPING_DOMAIN] = pGrouping;

	m_eActiveGrouping = BUILDING_GROUPING_WONDER_TYPE;
}

BuildingGroupingList::~BuildingGroupingList()
{
	for (int i = 0; i < NUM_BUILDING_GROUPING; i++)
	{
		delete m_apBuildingGrouping[i];
	}
}

BuildingGroupingTypes BuildingGroupingList::getActiveGrouping() const
{
	return m_eActiveGrouping;
}

void BuildingGroupingList::setCity(CvCity *pCity)
{
	m_pCity = pCity;
}

void BuildingGroupingList::setPlayer(CvPlayer *pPlayer)
{
	m_pPlayer = pPlayer;
}

bool BuildingGroupingList::setActiveGrouping(BuildingGroupingTypes eActiveGrouping)
{
	FASSERT_BOUNDS(0, NUM_BUILDING_GROUPING, eActiveGrouping)
	const bool bChanged = m_eActiveGrouping != eActiveGrouping;
	m_eActiveGrouping = eActiveGrouping;
	return bChanged;
}

int BuildingGroupingList::getGroup(BuildingTypes eBuilding) const
{
	return m_apBuildingGrouping[m_eActiveGrouping]->getGroup(m_pPlayer, m_pCity, eBuilding);
}
