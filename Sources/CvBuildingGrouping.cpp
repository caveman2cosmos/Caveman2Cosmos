//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingGrouping.cpp
//
//  PURPOSE: Grouping classes for buildings
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

int BuildingGroupingBase::getGroup(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	const int iInverse = m_bInvert ? -1 : 1;
	return iInverse * getGroupBuilding(pPlayer, pCity, eBuilding);
}

BuildingGroupingBase::~BuildingGroupingBase()
{
}

int BuildingGroupingSingle::getGroupBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	return 1;
}

int BuildingGroupingWonderType::getGroupBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	if (!isLimitedWonder(eBuilding))
		return 0;
	if (isNationalWonder(eBuilding))
		return 1;
	if (isWorldWonder(eBuilding))
		return 3;
	return 2;
}

int BuildingGroupingFilters::getGroupBuilding(CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding)
{
	int iSize = m_apFilters.size();
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

BuildingGroupingList::BuildingGroupingList(CvPlayer *pPlayer, CvCity *pCity)
{
	m_pPlayer = pPlayer;
	m_pCity = pCity;
	
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

int BuildingGroupingList::getNumGrouping()
{
	return NUM_BUILDING_GROUPING;
}

BuildingGroupingTypes BuildingGroupingList::getActiveGrouping()
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
	FAssertMsg(eActiveGrouping < NUM_BUILDING_GROUPING, "Index out of bounds");
	FAssertMsg(eActiveGrouping > -1, "Index out of bounds");
	bool bChanged = m_eActiveGrouping != eActiveGrouping;
	m_eActiveGrouping = eActiveGrouping;
	return bChanged;
}

int BuildingGroupingList::getGroup(BuildingTypes eBuilding)
{
	return m_apBuildingGrouping[m_eActiveGrouping]->getGroup(m_pPlayer, m_pCity, eBuilding);
}
